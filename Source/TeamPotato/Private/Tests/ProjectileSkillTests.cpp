#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Combat/ProjectileSkillAbility.h"
#include "Combat/TurretProjectile.h"
#include "Combat/CombatAbilitySystemComponent.h"
#include "Combat/CombatEffects.h"
#include "Combat/CombatTags.h"
#include "Combat/RangedTargetingComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Player/TestCharacter.h"
#include "UObject/UnrealType.h"
#include <limits>

namespace
{
    struct FProjectileSkillWorld
    {
        UWorld* World;
        uint64 InitialFrame = GFrameCounter;
        ATestCharacter* Player;
        UCombatAbilitySystemComponent* ASC;
        FGameplayAbilitySpecHandle Handle;
        UProjectileSkillAbility* Skill;

        FProjectileSkillWorld(TSubclassOf<ATestCharacter> PlayerClass = ATestCharacter::StaticClass(),
            TSubclassOf<UProjectileSkillAbility> AbilityClass = UProjectileSkillAbility::StaticClass())
        {
            World = UWorld::CreateWorld(EWorldType::GamePreview, false);
            GEngine->CreateNewWorldContext(EWorldType::GamePreview).SetCurrentWorld(World);
            World->InitializeActorsForPlay(FURL());
            Player = World->SpawnActor<ATestCharacter>(PlayerClass, FVector(0, 0, 100), FRotator::ZeroRotator);
            ASC = CastChecked<UCombatAbilitySystemComponent>(Player->GetAbilitySystemComponent());
            // Character BPs grant their configured skill in PostInitializeComponents, without UI BeginPlay.
            const FGameplayAbilitySpec* Existing = ASC->FindAbilitySpecFromClass(AbilityClass);
            Handle = Existing ? Existing->Handle : ASC->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, INDEX_NONE, Player));
            Skill = CastChecked<UProjectileSkillAbility>(ASC->FindAbilitySpecFromHandle(Handle)->GetPrimaryInstance());
        }

        ~FProjectileSkillWorld()
        {
            World->DestroyWorld(false);
            GEngine->DestroyWorldContext(World);
            GFrameCounter = InitialFrame;
        }

        void UseTestCooldown()
        {
            // A real duration GE with granted tags exercises GAS's normal cooldown contract.
            auto* Property = FindFProperty<FClassProperty>(UGameplayAbility::StaticClass(), TEXT("CooldownGameplayEffectClass"));
            check(Property);
            Property->SetObjectPropertyValue_InContainer(Skill, UCombatHitInvincibilityEffect::StaticClass());
        }

        int32 ShotCount() const
        {
            int32 Count = 0;
            for (TActorIterator<ATurretProjectile> It(World); It; ++It)
                if (!It->IsActorBeingDestroyed()) ++Count;
            return Count;
        }

        ATurretProjectile* FirstShot() const
        {
            for (TActorIterator<ATurretProjectile> It(World); It; ++It)
                if (!It->IsActorBeingDestroyed()) return *It;
            return nullptr;
        }

        void Advance(float Seconds)
        {
            for (float Time = 0; Time < Seconds; Time += 0.025f)
            {
                for (TActorIterator<ATurretProjectile> It(World); It; ++It)
                    if (!It->HasActorBegunPlay()) It->DispatchBeginPlay();
                ++GFrameCounter;
                World->Tick(LEVELTICK_All, 0.025f);
            }
        }
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProjectileSkillFireTest, "TeamPotato.Combat.ProjectileSkill.FireDamageAndCooldown",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FProjectileSkillFireTest::RunTest(const FString& Parameters)
{
    FProjectileSkillWorld Fixture;
    Fixture.UseTestCooldown();
    auto* Enemy = Fixture.World->SpawnActor<AEnemyCharacter>(FVector(500, 0, 130), FRotator::ZeroRotator);
    const float Before = Enemy->GetCombatHealth();
    TestTrue(TEXT("Skill activates through GAS"), Fixture.ASC->TryActivateAbility(Fixture.Handle, false));
    TestEqual(TEXT("One projectile spawned"), Fixture.ShotCount(), 1);
    TestEqual(TEXT("No damage before impact"), Enemy->GetCombatHealth(), Before);
    auto* Shot = Fixture.FirstShot();
    if (!TestNotNull(TEXT("Live projectile remains after instant GA ends"), Shot)) return false;
    const float Damage = Shot->Damage;
    TestTrue(TEXT("Owner is the caster"), Shot->GetOwner() == Fixture.Player);
    TestTrue(TEXT("Instigator is the caster"), Shot->GetInstigator() == Fixture.Player);
    TestTrue(TEXT("Spawn offset uses character space"), Shot->GetActorLocation().Equals(FVector(100, 0, 130), 0.01f));
    TestFalse(TEXT("GA does not stay active while projectile flies"), Fixture.ASC->FindAbilitySpecFromHandle(Fixture.Handle)->IsActive());
    TestTrue(TEXT("Cooldown GE committed"), Fixture.ASC->HasMatchingGameplayTag(CombatTags::State_Invincible));
    TestFalse(TEXT("Cooldown prevents repeated input"), Fixture.ASC->TryActivateAbility(Fixture.Handle, false));
    TestEqual(TEXT("Cooldown input creates no extra projectile"), Fixture.ShotCount(), 1);

    Fixture.Advance(0.4f);
    TestEqual(TEXT("Projectile impact applies GAS damage once"), Enemy->GetCombatHealth(), Before - Damage);
    TestEqual(TEXT("Projectile is consumed on impact"), Fixture.ShotCount(), 0);
    TestFalse(TEXT("Player is not damaged by own projectile"), Fixture.ASC->GetHealth() < Fixture.ASC->GetMaxHealth());
    Fixture.Advance(0.7f); // The test cooldown GE lasts one second.
    TestFalse(TEXT("Cooldown expires"), Fixture.ASC->HasMatchingGameplayTag(CombatTags::State_Invincible));
    TestTrue(TEXT("Skill can fire again after cooldown"), Fixture.ASC->TryActivateAbility(Fixture.Handle, false));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProjectileSkillCloseTargetTest, "TeamPotato.Combat.ProjectileSkill.PointBlankTarget",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FProjectileSkillCloseTargetTest::RunTest(const FString& Parameters)
{
    FProjectileSkillWorld Fixture;
    FActorSpawnParameters Spawn;
    Spawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    auto* Enemy = Fixture.World->SpawnActor<AEnemyCharacter>(FVector(80, 0, 130), FRotator::ZeroRotator, Spawn);
    const float Before = Enemy->GetCombatHealth();
    TestTrue(TEXT("An enemy inside the usual muzzle distance does not block casting"),
        Fixture.ASC->TryActivateAbility(Fixture.Handle, false));
    auto* Shot = Fixture.FirstShot();
    if (!TestNotNull(TEXT("Point-blank shot is spawned"), Shot)) return false;
    TestTrue(TEXT("Projectile starts before the close enemy"), Shot->GetActorLocation().X < Enemy->GetActorLocation().X);
    const float Damage = Shot->Damage;
    Fixture.Advance(0.15f);
    TestEqual(TEXT("Point-blank enemy takes one hit"), Enemy->GetCombatHealth(), Before - Damage);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProjectileSkillGateTest, "TeamPotato.Combat.ProjectileSkill.SpawnSafetyAndStateGates",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FProjectileSkillGateTest::RunTest(const FString& Parameters)
{
    FProjectileSkillWorld Fixture;
    Fixture.UseTestCooldown();
    AActor* Wall = Fixture.World->SpawnActor<AActor>();
    auto* Box = NewObject<UBoxComponent>(Wall);
    Wall->AddInstanceComponent(Box);
    Wall->SetRootComponent(Box);
    Box->InitBoxExtent(FVector(10, 50, 50));
    Box->SetCollisionProfileName(TEXT("BlockAll"));
    Box->RegisterComponent();
    Wall->SetActorLocation(FVector(50, 0, 120));
    TestFalse(TEXT("Wall between caster and muzzle blocks cast"), Fixture.ASC->TryActivateAbility(Fixture.Handle, false));
    TestEqual(TEXT("Blocked cast spawns no projectile"), Fixture.ShotCount(), 0);
    TestFalse(TEXT("Blocked cast does not spend cooldown"), Fixture.ASC->HasMatchingGameplayTag(CombatTags::State_Invincible));
    Wall->Destroy();

    Fixture.ASC->AddLooseGameplayTag(CombatTags::State_AttackBlocked);
    TestFalse(TEXT("Electric attack block prevents projectile skill"), Fixture.ASC->TryActivateAbility(Fixture.Handle, false));
    Fixture.ASC->RemoveLooseGameplayTag(CombatTags::State_AttackBlocked);
    Fixture.Skill->ProjectileClass = nullptr;
    TestFalse(TEXT("Missing projectile class safely rejects cast"), Fixture.ASC->TryActivateAbility(Fixture.Handle, false));
    Fixture.Skill->ProjectileClass = ATurretProjectile::StaticClass();
    Fixture.Skill->SpawnOffset.X = std::numeric_limits<float>::quiet_NaN();
    TestFalse(TEXT("Invalid spawn coordinates safely reject cast"), Fixture.ASC->TryActivateAbility(Fixture.Handle, false));
    Fixture.Skill->SpawnOffset = FVector(100, 0, 30);
    TestEqual(TEXT("Rejected casts leave no actors behind"), Fixture.ShotCount(), 0);

    Fixture.Player->SetActorRotation(FRotator(0, 90, 0));
    TestTrue(TEXT("Valid cast succeeds after failures"), Fixture.ASC->TryActivateAbility(Fixture.Handle, false));
    auto* Shot = Fixture.FirstShot();
    if (!TestNotNull(TEXT("Rotated cast creates projectile"), Shot)) return false;
    TestTrue(TEXT("Muzzle follows yaw"), Shot->GetActorLocation().Equals(FVector(0, 100, 130), 0.01f));
    TestTrue(TEXT("Velocity follows yaw"), Shot->Movement->Velocity.GetSafeNormal().Equals(FVector::RightVector, 0.01f));
    Fixture.ASC->MarkDead();
    TestFalse(TEXT("Dead caster cannot activate"), Fixture.ASC->TryActivateAbility(Fixture.Handle, false));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProjectileSkillLockOnTest, "TeamPotato.Combat.ProjectileSkill.LockOnAndFallback",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FProjectileSkillLockOnTest::RunTest(const FString& Parameters)
{
    FProjectileSkillWorld Fixture;
    auto* Enemy = Fixture.World->SpawnActor<AEnemyCharacter>(FVector(500, 200, 100), FRotator::ZeroRotator);
    const float Before = Enemy->GetCombatHealth();
    TestTrue(TEXT("Off-axis enemy allows skill activation"), Fixture.ASC->TryActivateAbility(Fixture.Handle, false));
    auto* Shot = Fixture.FirstShot();
    if (!TestNotNull(TEXT("Lock-on shot spawned"), Shot)) return false;
    TestTrue(TEXT("Skill velocity aims from muzzle to enemy center"),
        Shot->Movement->Velocity.GetSafeNormal().Equals((Enemy->GetActorLocation() - Shot->GetActorLocation()).GetSafeNormal(), 0.001f));
    TestTrue(TEXT("Skill spawn moves toward the selected enemy"), Shot->GetActorLocation().Y > 0.0f);
    const float Damage = Shot->Damage;
    Fixture.Advance(0.4f);
    TestEqual(TEXT("Off-axis target is physically hit through GAS"), Enemy->GetCombatHealth(), Before - Damage);

    Fixture.Player->GetRangedTargeting()->bEnableLockOn = false;
    TestTrue(TEXT("Lock-on can be disabled without disabling the skill"), Fixture.ASC->TryActivateAbility(Fixture.Handle, false));
    Shot = Fixture.FirstShot();
    if (!TestNotNull(TEXT("Fallback shot spawned"), Shot)) return false;
    TestTrue(TEXT("Disabled lock-on preserves forward fire"), Shot->Movement->Velocity.GetSafeNormal().Equals(FVector::ForwardVector, 0.001f));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRangeSkillAssetTest, "TeamPotato.Combat.ProjectileSkill.RangeSkillAssetIntegration",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRangeSkillAssetTest::RunTest(const FString& Parameters)
{
    UClass* PlayerClass = LoadClass<ATestCharacter>(nullptr,
        TEXT("/Game/BluePrints/Test/Player/BP_PlayerGunner.BP_PlayerGunner_C"));
    UClass* SkillClass = LoadClass<UProjectileSkillAbility>(nullptr,
        TEXT("/Game/GameplayAbility/GA_RangeSkill.GA_RangeSkill_C"));
    UClass* ProjectileClass = LoadClass<ATurretProjectile>(nullptr,
        TEXT("/Game/BluePrints/Weapon/Projectiles/BP_RangeSkillProjectile.BP_RangeSkillProjectile_C"));
    UClass* CooldownClass = LoadClass<UGameplayEffect>(nullptr,
        TEXT("/Game/Effect/GE_RangeSkillCoolDown.GE_RangeSkillCoolDown_C"));
    if (!TestNotNull(TEXT("Gunner character BP loads"), PlayerClass) ||
        !TestNotNull(TEXT("Range skill uses the native projectile ability parent"), SkillClass) ||
        !TestNotNull(TEXT("Range skill projectile BP loads"), ProjectileClass) ||
        !TestNotNull(TEXT("Range skill cooldown GE loads"), CooldownClass)) return false;

    const auto* SkillProperty = FindFProperty<FClassProperty>(ATestCharacter::StaticClass(), TEXT("SkillAbilityClass"));
    if (!TestTrue(TEXT("Gunner BP assigns GA_RangeSkill to its input skill"), SkillProperty &&
        SkillProperty->GetObjectPropertyValue_InContainer(PlayerClass->GetDefaultObject()) == SkillClass)) return false;

    FProjectileSkillWorld Fixture(PlayerClass, SkillClass);
    TestEqual(TEXT("GA uses the dedicated skill projectile BP"), Fixture.Skill->ProjectileClass.Get(), ProjectileClass);
    const UGameplayEffect* Cooldown = Fixture.Skill->GetCooldownGameplayEffect();
    if (!TestNotNull(TEXT("GA has a cooldown effect"), Cooldown)) return false;
    TestEqual(TEXT("GA uses the dedicated cooldown GE"), Cooldown->GetClass(), CooldownClass);
    const FGameplayTag CooldownTag = FGameplayTag::RequestGameplayTag(TEXT("Cooldown.RangeSkillCoolDown"), false);
    if (!TestTrue(TEXT("Range skill cooldown tag is registered"), CooldownTag.IsValid())) return false;

    auto* Enemy = Fixture.World->SpawnActor<AEnemyCharacter>(FVector(500, 200, 100), FRotator::ZeroRotator);
    if (!TestNotNull(TEXT("Off-axis target spawned"), Enemy)) return false;
    const float Before = Enemy->GetCombatHealth();
    if (!TestTrue(TEXT("Real Gunner skill input activates the configured GA"), Fixture.Player->TryUseCharacterSkill())) return false;
    auto* Shot = Fixture.FirstShot();
    if (!TestNotNull(TEXT("Configured projectile spawned"), Shot)) return false;
    TestEqual(TEXT("Spawned projectile is the configured BP"), Shot->GetClass(), ProjectileClass);
    TestTrue(TEXT("Projectile has a visible mesh asset"), Shot->Mesh && Shot->Mesh->GetStaticMesh() &&
        Shot->Mesh->IsVisible() && !Shot->Mesh->bHiddenInGame);
    TestTrue(TEXT("Projectile owner is the real Gunner"), Shot->GetOwner() == Fixture.Player && Shot->GetInstigator() == Fixture.Player);
    TestTrue(TEXT("GA corrects its projectile direction toward an off-axis enemy"),
        Shot->Movement->Velocity.GetSafeNormal().Equals((Enemy->GetActorLocation() - Shot->GetActorLocation()).GetSafeNormal(), 0.001f));
    TestTrue(TEXT("Cooldown starts at five seconds"), FMath::IsNearlyEqual(Fixture.Skill->GetCooldownTimeRemaining(), 5.0f, 0.05f));
    TestTrue(TEXT("Cooldown grants its own range skill tag"), Fixture.ASC->HasMatchingGameplayTag(CooldownTag));
    TestFalse(TEXT("Repeated skill input is blocked during cooldown"), Fixture.Player->TryUseCharacterSkill());
    TestEqual(TEXT("Repeated input creates no additional shot"), Fixture.ShotCount(), 1);
    const float Damage = Shot->Damage;
    Fixture.Advance(0.4f);
    TestEqual(TEXT("Actual skill projectile damages its off-axis target through GAS"), Enemy->GetCombatHealth(), Before - Damage);
    TestEqual(TEXT("Projectile disappears after impact"), Fixture.ShotCount(), 0);

    Enemy->Destroy();
    Fixture.Advance(4.4f);
    TestFalse(TEXT("Skill stays blocked until the full cooldown expires"), Fixture.Player->TryUseCharacterSkill());
    Fixture.Advance(0.4f);
    TestFalse(TEXT("Cooldown tag expires automatically"), Fixture.ASC->HasMatchingGameplayTag(CooldownTag));
    Fixture.Player->SetActorRotation(FRotator(0, 90, 0));
    if (!TestTrue(TEXT("Skill input works again after cooldown"), Fixture.Player->TryUseCharacterSkill())) return false;
    Shot = Fixture.FirstShot();
    if (!TestNotNull(TEXT("No-target shot spawned"), Shot)) return false;
    TestTrue(TEXT("Without a target the skill fires in the character's current forward direction"),
        Shot->Movement->Velocity.GetSafeNormal().Equals(Fixture.Player->GetActorForwardVector(), 0.001f));
    Fixture.Advance(5.2f);
    TestEqual(TEXT("A missed skill projectile expires after its configured lifetime"), Fixture.ShotCount(), 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRangeSkillCastMontageTest, "TeamPotato.Combat.ProjectileSkill.OptionalCastMontage",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRangeSkillCastMontageTest::RunTest(const FString& Parameters)
{
    UClass* PlayerClass = LoadClass<ATestCharacter>(nullptr,
        TEXT("/Game/BluePrints/Test/Player/BP_PlayerGunner.BP_PlayerGunner_C"));
    UClass* SkillClass = LoadClass<UProjectileSkillAbility>(nullptr,
        TEXT("/Game/GameplayAbility/GA_RangeSkill.GA_RangeSkill_C"));
    UAnimMontage* Montage = LoadObject<UAnimMontage>(nullptr,
        TEXT("/Game/Animations/Montage/Gun/AM_Attack_Gun_Revenant.AM_Attack_Gun_Revenant"));
    if (!TestNotNull(TEXT("Gunner BP loads"), PlayerClass) ||
        !TestNotNull(TEXT("Range skill GA loads"), SkillClass) ||
        !TestNotNull(TEXT("Compatible Gunner montage loads"), Montage)) return false;

    FProjectileSkillWorld Fixture(PlayerClass, SkillClass);
    UAnimInstance* Anim = Fixture.Player->GetMesh()->GetAnimInstance();
    if (!TestNotNull(TEXT("Actual Gunner animation instance exists"), Anim)) return false;
    auto* MontageProperty = FindFProperty<FObjectPropertyBase>(ATestCharacter::StaticClass(), TEXT("SkillCastMontage"));
    if (!TestNotNull(TEXT("Character exposes its optional skill montage"), MontageProperty)) return false;
    // Configure only this test actor; the character BP and montage assets remain untouched.
    MontageProperty->SetObjectPropertyValue_InContainer(Fixture.Player, Montage);
    if (!TestTrue(TEXT("Configured skill activates through the real input entry point"), Fixture.Player->TryUseCharacterSkill())) return false;
    TestEqual(TEXT("Optional montage keeps the immediate single projectile launch"), Fixture.ShotCount(), 1);
    TestFalse(TEXT("Projectile GA ends without waiting for the montage"), Fixture.ASC->FindAbilitySpecFromHandle(Fixture.Handle)->IsActive());
    TestTrue(TEXT("Cast montage remains playing after the GA has ended"), Anim->Montage_IsPlaying(Montage));

    Anim->Montage_Stop(0.0f, Montage);
    TestFalse(TEXT("Test montage is stopped before rejected input"), Anim->Montage_IsPlaying(Montage));
    TestFalse(TEXT("Cooldown rejects another skill use"), Fixture.Player->TryUseCharacterSkill());
    TestFalse(TEXT("Rejected use does not start the cast montage"), Anim->Montage_IsPlaying(Montage));
    TestEqual(TEXT("Rejected use creates no additional projectile"), Fixture.ShotCount(), 1);
    return true;
}

#endif
