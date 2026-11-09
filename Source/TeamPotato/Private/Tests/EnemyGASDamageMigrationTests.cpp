#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Combat/CombatAbilitySystemComponent.h"
#include "Combat/CombatEffects.h"
#include "Combat/CombatTags.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Enemy/Bullet/EnemyProjectile.h"
#include "Enemy/BulletBase.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/EnemyWeaponBase.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameplayEffect.h"
#include "Player/TestCharacter.h"

namespace
{
    struct FEnemyDamageMigrationWorld
    {
        UWorld* World;

        FEnemyDamageMigrationWorld()
        {
            World = UWorld::CreateWorld(EWorldType::GamePreview, false);
            GEngine->CreateNewWorldContext(EWorldType::GamePreview).SetCurrentWorld(World);
            World->InitializeActorsForPlay(FURL());
        }

        ~FEnemyDamageMigrationWorld()
        {
            World->DestroyWorld(false);
            GEngine->DestroyWorldContext(World);
        }
    };

    bool VerifyEnemyBlueprintDamage(FAutomationTestBase& Test, const TCHAR* AssetPath, bool bProjectile, float BaseDamage)
    {
        UClass* DamageClass = LoadClass<AActor>(nullptr, AssetPath);
        if (!Test.TestNotNull(TEXT("Actual enemy damage Blueprint loads"), DamageClass)) return false;

        FEnemyDamageMigrationWorld Fixture;
        FActorSpawnParameters Params;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        auto* Enemy = Fixture.World->SpawnActor<AEnemyCharacter>(FVector(-2000, 0, 100), FRotator::ZeroRotator, Params);
        auto* Player = Fixture.World->SpawnActor<ATestCharacter>(FVector(0, 0, 100), FRotator::ZeroRotator, Params);
        if (!Test.TestNotNull(TEXT("Attacker spawns"), Enemy) || !Test.TestNotNull(TEXT("Player spawns"), Player)) return false;
        Fixture.World->SpawnActor<APlayerController>()->Possess(Player);
        Enemy->GetCharacterMovement()->DisableMovement();
        Player->GetCharacterMovement()->DisableMovement();
        Params.Owner = Enemy;
        Params.Instigator = Enemy;
        AActor* DamageActor = Fixture.World->SpawnActor<AActor>(DamageClass, FVector(2000, 0, 100), FRotator::ZeroRotator, Params);
        if (!Test.TestNotNull(TEXT("Real weapon/projectile spawns"), DamageActor)) return false;
        auto* Weapon = Cast<AEnemyWeaponBase>(DamageActor);
        auto* Projectile = Cast<ABulletBase>(DamageActor);
        if (!Test.TestTrue(TEXT("Correct native damage actor base"), bProjectile ? Projectile != nullptr : Weapon != nullptr)) return false;
        TArray<UCapsuleComponent*> Capsules;
        DamageActor->GetComponents(Capsules);
        if (!Test.TestTrue(TEXT("Real Blueprint has a collision capsule"), Capsules.Num() > 0)) return false;
        UCapsuleComponent* HitCapsule = Capsules[0];

        // Start only the damage actor. Player/enemy BeginPlay depends on level UI and game-instance subsystems.
        DamageActor->DispatchBeginPlay();
        Fixture.World->SetBegunPlay(true);
        auto* SourceASC = CastChecked<UCombatAbilitySystemComponent>(Enemy->GetAbilitySystemComponent());
        auto* TargetASC = CastChecked<UCombatAbilitySystemComponent>(Player->GetAbilitySystemComponent());
        const FGameplayTag ReductionTag = FGameplayTag::RequestGameplayTag(TEXT("State.DamageReduction"));
        int32 DamageNotifications = 0;
        AActor* LastInstigator = nullptr;
        AActor* LastCauser = nullptr;
        float LastRequestedDamage = -1.0f;
        const FDelegateHandle DamageDelegate = TargetASC->OnCombatDamageReceived.AddLambda(
            [&](float Damage, const FGameplayEffectContextHandle& Context)
            {
                ++DamageNotifications;
                LastInstigator = Context.GetOriginalInstigator();
                LastCauser = Context.GetEffectCauser();
            });
        const FDelegateHandle EffectDelegate = TargetASC->OnGameplayEffectAppliedDelegateToSelf.AddLambda(
            [&](UAbilitySystemComponent*, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle)
            {
                if (Spec.Def && Spec.Def->GetClass() == UCombatDamageEffect::StaticClass())
                    LastRequestedDamage = Spec.GetSetByCallerMagnitude(CombatTags::Data_Damage, false, -1.0f);
            });

        auto TriggerOverlap = [&]()
        {
            if (Projectile)
            {
                Projectile->Deactivate();
                Projectile->Launch(Player->GetActorLocation() - FVector(200, 0, 0), FVector::ForwardVector, 1000.0f);
                Projectile->SetActorLocation(Player->GetActorLocation());
            }
            else
            {
                Weapon->DisableAttackCollision();
                HitCapsule->SetWorldLocation(Player->GetActorLocation());
                Weapon->EnableAttackCollision();
            }
            HitCapsule->UpdateOverlaps();
        };
        auto ClearHitInvincibility = [&]()
        {
            TargetASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(CombatTags::State_Invincible));
        };

        // The old BP ApplyDamage nodes did not supply DamageCauser. An empty attacker GE exposes
        // that old target-side fallback even though both paths eventually reach GAS health.
        SourceASC->DamageEffectClass = UGameplayEffect::StaticClass();
        const float InitialHealth = TargetASC->GetHealth();
        TriggerOverlap();
        Test.TestEqual(TEXT("Blueprint honors the monster's configured damage GE"), TargetASC->GetHealth(), InitialHealth);
        Test.TestEqual(TEXT("An empty attacker GE emits no successful damage notification"), DamageNotifications, 0);
        ClearHitInvincibility();

        SourceASC->DamageEffectClass = UCombatDamageEffect::StaticClass();
        TargetASC->AddLooseGameplayTag(ReductionTag);
        const float BeforeReducedHit = TargetASC->GetHealth();
        TriggerOverlap();
        Test.TestTrue(TEXT("Real overlap retains damage reduction"),
            FMath::IsNearlyEqual(BeforeReducedHit - TargetASC->GetHealth(), BaseDamage * 0.7f, 0.001f));
        Test.TestEqual(TEXT("Blueprint damage travels through Data.Damage"), LastRequestedDamage, BaseDamage);
        Test.TestEqual(TEXT("Damage context identifies the monster"), LastInstigator, static_cast<AActor*>(Enemy));
        Test.TestEqual(TEXT("Damage context identifies the actual weapon/projectile"), LastCauser, DamageActor);
        Test.TestEqual(TEXT("One accepted overlap emits one damage notification"), DamageNotifications, 1);
        const float AfterHit = TargetASC->GetHealth();
        TriggerOverlap();
        Test.TestEqual(TEXT("Immediate repeated overlap is blocked by hit invincibility"), TargetASC->GetHealth(), AfterHit);
        Test.TestEqual(TEXT("Invincible overlap emits no extra damage notification"), DamageNotifications, 1);

        ClearHitInvincibility();
        TargetASC->RemoveLooseGameplayTag(ReductionTag);
        TriggerOverlap();
        Test.TestTrue(TEXT("Unbuffed overlap preserves the previous Blueprint damage amount"),
            FMath::IsNearlyEqual(AfterHit - TargetASC->GetHealth(), BaseDamage, 0.001f));
        Test.TestEqual(TEXT("A later accepted hit emits its own notification"), DamageNotifications, 2);
        TargetASC->OnCombatDamageReceived.Remove(DamageDelegate);
        TargetASC->OnGameplayEffectAppliedDelegateToSelf.Remove(EffectDelegate);
        return true;
    }
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEnemyMeleeGASMigrationTest, "TeamPotato.Combat.Enemy.GASMigration.Melee",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEnemyMeleeGASMigrationTest::RunTest(const FString& Parameters)
{
    return VerifyEnemyBlueprintDamage(*this, TEXT("/Game/Enemy/Blueprint/Weapon/BP_Sword.BP_Sword_C"), false, 5.0f);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEnemyProjectileGASMigrationTest, "TeamPotato.Combat.Enemy.GASMigration.Projectile",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEnemyProjectileGASMigrationTest::RunTest(const FString& Parameters)
{
    return VerifyEnemyBlueprintDamage(*this, TEXT("/Game/Enemy/Blueprint/Weapon/BP_Arrow.BP_Arrow_C"), true, 6.0f);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBossProjectileGASMigrationTest, "TeamPotato.Combat.Enemy.GASMigration.BossProjectile",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBossProjectileGASMigrationTest::RunTest(const FString& Parameters)
{
    UClass* ProjectileClass = LoadClass<AEnemyProjectile>(nullptr,
        TEXT("/Game/Enemy/Blueprint/Bullet/LastBoss/BP_MagmaBall.BP_MagmaBall_C"));
    if (!TestNotNull(TEXT("Real boss projectile Blueprint loads"), ProjectileClass)) return false;
    FEnemyDamageMigrationWorld Fixture;
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    auto* Enemy = Fixture.World->SpawnActor<AEnemyCharacter>(FVector(-2000, 0, 100), FRotator::ZeroRotator, Params);
    auto* Player = Fixture.World->SpawnActor<ATestCharacter>(FVector(0, 0, 100), FRotator::ZeroRotator, Params);
    if (!TestNotNull(TEXT("Boss source spawns"), Enemy) || !TestNotNull(TEXT("Player spawns"), Player)) return false;
    Fixture.World->SpawnActor<APlayerController>()->Possess(Player);
    Enemy->GetCharacterMovement()->DisableMovement();
    Player->GetCharacterMovement()->DisableMovement();

    // Boss patterns can create a second actor which then spawns projectiles. No pawn
    // instigator is supplied: ownership must carry attribution through both links.
    Params.Owner = Enemy;
    AActor* Pattern = Fixture.World->SpawnActor<AActor>(Params);
    if (!TestNotNull(TEXT("Intermediate pattern actor spawns"), Pattern)) return false;
    Params.Owner = Pattern;
    Params.Instigator = nullptr;
    auto* Projectile = Fixture.World->SpawnActor<AEnemyProjectile>(ProjectileClass, FVector(2000, 0, 100), FRotator::ZeroRotator, Params);
    if (!TestNotNull(TEXT("Boss projectile spawns"), Projectile)) return false;
    Projectile->DispatchBeginPlay();
    Fixture.World->SetBegunPlay(true);
    auto* SourceASC = CastChecked<UCombatAbilitySystemComponent>(Enemy->GetAbilitySystemComponent());
    auto* TargetASC = CastChecked<UCombatAbilitySystemComponent>(Player->GetAbilitySystemComponent());
    SourceASC->DamageEffectClass = UCombatDamageEffect::StaticClass();
    TargetASC->DamageEffectClass = UGameplayEffect::StaticClass();
    int32 DamageNotifications = 0;
    AActor* LastInstigator = nullptr;
    AActor* LastCauser = nullptr;
    const FDelegateHandle DamageDelegate = TargetASC->OnCombatDamageReceived.AddLambda(
        [&](float Damage, const FGameplayEffectContextHandle& Context)
        {
            ++DamageNotifications;
            LastInstigator = Context.GetOriginalInstigator();
            LastCauser = Context.GetEffectCauser();
        });
    const float BeforeHit = TargetASC->GetHealth();
    TestNull(TEXT("Projectile has no direct instigator override"), Projectile->GetInstigator());
    Projectile->SetActorLocation(Player->GetActorLocation());
    // Only one hit is needed; a projectile BP may destroy itself after processing it.
    if (IsValid(Projectile) && IsValid(Projectile->CollisionComp)) Projectile->CollisionComp->UpdateOverlaps();
    TestEqual(TEXT("Boss BP keeps its actual graph damage 7, not the unused native Damage default 10"),
        BeforeHit - TargetASC->GetHealth(), 7.0f);
    TestEqual(TEXT("Boss damage resolves the original monster through the pattern owner"), LastInstigator, static_cast<AActor*>(Enemy));
    TestEqual(TEXT("Boss damage retains the projectile as causer"), LastCauser, static_cast<AActor*>(Projectile));
    TestEqual(TEXT("Boss projectile emits one accepted damage event"), DamageNotifications, 1);
    TargetASC->OnCombatDamageReceived.Remove(DamageDelegate);
    return true;
}

#endif
