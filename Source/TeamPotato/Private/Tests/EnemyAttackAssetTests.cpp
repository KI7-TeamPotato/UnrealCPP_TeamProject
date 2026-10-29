#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Combat/CombatAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/EliteEnemy.h"
#include "Enemy/EnemyAxe.h"
#include "Enemy/EnemyWeaponBase.h"
#include "Enemy/ArcherEnemy.h"
#include "Enemy/BulletBase.h"
#include "Engine/Engine.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Player/TestCharacter.h"
#include "UObject/UnrealType.h"

namespace
{
    struct FEnemyAttackWorld
    {
        UWorld* World;
        uint64 InitialFrame = GFrameCounter;
        FEnemyAttackWorld()
        {
            World = UWorld::CreateWorld(EWorldType::GamePreview, false);
            GEngine->CreateNewWorldContext(EWorldType::GamePreview).SetCurrentWorld(World);
            World->InitializeActorsForPlay(FURL());
        }
        ~FEnemyAttackWorld()
        {
            World->DestroyWorld(false);
            GEngine->DestroyWorldContext(World);
            GFrameCounter = InitialFrame;
        }
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNewEnemyAttackAssetsTest, "TeamPotato.Combat.Enemy.ReplacedMeshAttacks",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FNewEnemyAttackAssetsTest::RunTest(const FString& Parameters)
{
    for (const TCHAR* Name : {TEXT("BP_Assassin"), TEXT("BP_EliteEnemy"), TEXT("BP_TreeEliteEnemy"),
        TEXT("BP_TreeEliteEnemy1"), TEXT("BP_Archer")})
    {
        FEnemyAttackWorld Fixture;
        UClass* Class = LoadClass<AEnemyCharacter>(nullptr,
            *FString::Printf(TEXT("/Game/Enemy/Blueprint/%s.%s_C"), Name, Name));
        if (!TestNotNull(FString(Name) + TEXT(" loads"), Class)) continue;
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        // Do not start level/UI/AI logic: exercise the real BP, mesh and notify directly.
        auto* Enemy = Fixture.World->SpawnActor<AEnemyCharacter>(Class, FVector(0, 0, 100), FRotator::ZeroRotator, Params);
        auto* Player = Fixture.World->SpawnActor<ATestCharacter>(FVector(1000, 0, 100), FRotator::ZeroRotator, Params);
        Fixture.World->SpawnActor<APlayerController>()->Possess(Player);
        Enemy->GetCharacterMovement()->DisableMovement();
        Player->GetCharacterMovement()->DisableMovement();
        if (!TestTrue(FString(Name) + TEXT(" has a compatible attack montage"), Enemy->AttackMontage &&
            Enemy->AttackMontage->GetSkeleton() == Enemy->GetMesh()->GetSkeletalMeshAsset()->GetSkeleton())) continue;
        TestTrue(FString(Name) + TEXT(" has a valid weapon attachment"),
            Enemy->GetMesh()->DoesSocketExist(Enemy->WeaponSocketName));
        Enemy->WieldWeapon();
        auto* Weapon = Enemy->CurrentWeapon;
        if (!TestNotNull(FString(Name) + TEXT(" equips weapon"), Weapon)) continue;
        Weapon->DispatchBeginPlay();
        TestTrue(TEXT("Damage Blueprint resolves the test player"), UGameplayStatics::GetPlayerCharacter(Enemy, 0) == Player);

        UAnimNotifyState* AttackNotify = nullptr;
        float NotifyDuration = 0.0f;
        for (const FAnimNotifyEvent& Event : Enemy->AttackMontage->Notifies)
        {
            if (Event.NotifyStateClass && Event.NotifyStateClass->GetClass()->GetName().StartsWith(TEXT("ANS_EnemyNotify_")))
            {
                AttackNotify = Event.NotifyStateClass;
                NotifyDuration = Event.GetDuration();
                break;
            }
        }
        if (!TestNotNull(FString(Name) + TEXT(" has an attack notify"), AttackNotify)) continue;

        if (auto* Archer = Cast<AArcherEnemy>(Enemy))
        {
            // Pool setup normally runs in BeginPlay. Seed real BP projectiles without starting AI/UI.
            FClassProperty* BulletProperty = FindFProperty<FClassProperty>(AArcherEnemy::StaticClass(), TEXT("BulletClass"));
            UClass* BulletClass = Cast<UClass>(BulletProperty->GetObjectPropertyValue_InContainer(Archer));
            auto* Bullet = Fixture.World->SpawnActor<ABulletBase>(BulletClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
            if (!TestNotNull(TEXT("Archer projectile asset"), Bullet)) continue;
            Bullet->DispatchBeginPlay();
            auto* PoolProperty = FindFProperty<FArrayProperty>(AArcherEnemy::StaticClass(), TEXT("BulletPool"));
            FScriptArrayHelper Pool(PoolProperty, PoolProperty->ContainerPtrToValuePtr<void>(Archer));
            const int32 Index = Pool.AddValue();
            CastFieldChecked<FObjectPropertyBase>(PoolProperty->Inner)->SetObjectPropertyValue(Pool.GetRawPtr(Index), Bullet);
            // Overlap delegates are suppressed by Unreal until the world is in play.
            // Start just the weapon/projectile; player/enemy BeginPlay needs level subsystems.
            Fixture.World->SetBegunPlay(true);
            const float Before = Cast<UCombatAbilitySystemComponent>(Player->GetAbilitySystemComponent())->GetHealth();
            AttackNotify->NotifyBegin(Enemy->GetMesh(), Enemy->AttackMontage, NotifyDuration, FAnimNotifyEventReference());
            TestTrue(TEXT("Archer notify launches a projectile"), Bullet->IsActive());
            Bullet->SetActorLocation(Player->GetActorLocation());
            Bullet->GetRootComponent()->UpdateComponentToWorld();
            Cast<UPrimitiveComponent>(Bullet->GetRootComponent())->UpdateOverlaps();
            TestTrue(TEXT("Archer projectile reduces player GAS health"),
                Cast<UCombatAbilitySystemComponent>(Player->GetAbilitySystemComponent())->GetHealth() < Before);
        }
        else
        {
            TArray<UCapsuleComponent*> Capsules;
            Weapon->GetComponents(Capsules);
            if (!TestTrue(FString(Name) + TEXT(" has a melee hit capsule"), !Capsules.IsEmpty())) continue;
            auto* HitCapsule = Capsules[0];
            Fixture.World->SetBegunPlay(true);
            const float Before = Cast<UCombatAbilitySystemComponent>(Player->GetAbilitySystemComponent())->GetHealth();
            Player->SetActorLocation(HitCapsule->GetComponentLocation());
            TestEqual(FString(Name) + TEXT(" does not damage outside attack window"),
                Cast<UCombatAbilitySystemComponent>(Player->GetAbilitySystemComponent())->GetHealth(), Before);
            AttackNotify->NotifyBegin(Enemy->GetMesh(), Enemy->AttackMontage, NotifyDuration, FAnimNotifyEventReference());
            HitCapsule->UpdateOverlaps();
            TestTrue(FString(Name) + TEXT(" attack enables collision"), HitCapsule->GetCollisionEnabled() != ECollisionEnabled::NoCollision);
            TestTrue(FString(Name) + TEXT(" melee overlap reduces player GAS health"),
                Cast<UCombatAbilitySystemComponent>(Player->GetAbilitySystemComponent())->GetHealth() < Before);
            AttackNotify->NotifyEnd(Enemy->GetMesh(), Enemy->AttackMontage, FAnimNotifyEventReference());
            TestEqual(FString(Name) + TEXT(" attack end disables collision"), HitCapsule->GetCollisionEnabled(), ECollisionEnabled::NoCollision);
        }
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGolemSpecialAttackTest, "TeamPotato.Combat.Enemy.GolemSpecialThrow",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGolemSpecialAttackTest::RunTest(const FString& Parameters)
{
    FEnemyAttackWorld Fixture;
    UClass* Class = LoadClass<AEliteEnemy>(nullptr, TEXT("/Game/Enemy/Blueprint/BP_EliteEnemy.BP_EliteEnemy_C"));
    if (!TestNotNull(TEXT("Golem Blueprint loads"), Class)) return false;
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    auto* Enemy = Fixture.World->SpawnActor<AEliteEnemy>(Class, FVector(0, 0, 100), FRotator::ZeroRotator, Params);
    auto* Player = Fixture.World->SpawnActor<ATestCharacter>(FVector(600, 0, 100), FRotator::ZeroRotator, Params);
    Fixture.World->SpawnActor<APlayerController>()->Possess(Player);
    Enemy->WieldWeapon();
    auto* Axe = Cast<AEnemyAxe>(Enemy->CurrentWeapon);
    if (!TestNotNull(TEXT("Golem equips throwable axe"), Axe)) return false;
    Axe->DispatchBeginPlay();
    Fixture.World->SetBegunPlay(true);

    // Use the same entry point as BTT_Grab; inspect the montage that actually starts.
    Enemy->GrabAttack();
    auto* Montage = Enemy->GetCurrentMontage();
    if (!TestNotNull(TEXT("AI special attack starts a montage"), Montage)) return false;
    UAnimNotify* ThrowNotify = nullptr;
    for (const FAnimNotifyEvent& Event : Montage->Notifies)
    {
        if (Event.Notify && Event.Notify->GetClass()->GetName() == TEXT("AN_AxeNotify_C")) ThrowNotify = Event.Notify;
        TestFalse(TEXT("Special throw does not enable a second melee damage capsule"),
            Event.NotifyStateClass && Event.NotifyStateClass->GetClass()->GetName() == TEXT("ANS_EnemyNotify_Elite_C"));
    }
    if (!TestNotNull(TEXT("Special montage has the axe release notify"), ThrowNotify)) return false;
    const FVector Origin = Axe->GetActorLocation();
    auto* PlayerASC = Cast<UCombatAbilitySystemComponent>(Player->GetAbilitySystemComponent());
    const float Health = PlayerASC->GetHealth();
    ThrowNotify->Notify(Enemy->GetMesh(), Montage, FAnimNotifyEventReference());
    TestNull(TEXT("Release notify detaches axe from hand"), Axe->GetAttachParentActor());

    // Run just the thrown weapon: no level, AI or player BeginPlay dependencies are needed.
    AActor* ThrownActor = Axe;
    bool bCaughtPlayer = false;
    for (int32 Frame = 0; Frame < 300 && !Axe->GetAttachParentActor(); ++Frame)
    {
        ThrownActor->Tick(0.01f);
        bCaughtPlayer |= Player->GetCharacterMovement()->MovementMode == MOVE_None;
    }
    TestTrue(TEXT("Axe actually travels from the release position"), !Axe->GetActorLocation().Equals(Origin) || bCaughtPlayer);
    TestTrue(TEXT("Thrown axe reduces player GAS health"), PlayerASC->GetHealth() < Health);
    TestTrue(TEXT("Axe grabs the successfully hit player"), bCaughtPlayer);
    TestTrue(TEXT("Axe returns to its owner"), Axe->GetAttachParentActor() == Enemy);
    TestEqual(TEXT("Player movement restored after return"), Player->GetCharacterMovement()->MovementMode.GetValue(), MOVE_Walking);
    return true;
}

#endif
