#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Combat/CombatAbilitySystemComponent.h"
#include "Combat/CombatAttributeSet.h"
#include "Combat/CombatEffects.h"
#include "Combat/CombatFunctionLibrary.h"
#include "Combat/CombatTags.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Enemy/BossBase.h"
#include "Player/TestCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Item/Weapon/SwordWeaponActor.h"
#include "Data/WeaponDataAsset.h"
#include <limits>

namespace
{
    struct FCombatTestWorld
    {
        UWorld* World;
        uint64 InitialFrameCounter = GFrameCounter;

        FCombatTestWorld()
        {
            // No game-instance/UI/pool subsystems, but a real world and real GAS/timer execution.
            World = UWorld::CreateWorld(EWorldType::GamePreview, false);
            GEngine->CreateNewWorldContext(EWorldType::GamePreview).SetCurrentWorld(World);
            World->InitializeActorsForPlay(FURL());
        }

        ~FCombatTestWorld()
        {
            World->DestroyWorld(false);
            GEngine->DestroyWorldContext(World);
            GFrameCounter = InitialFrameCounter;
        }

        UCombatAbilitySystemComponent* SpawnCombatant(float Health = 100.0f, float Invincibility = 0.0f)
        {
            AActor* Actor = World->SpawnActor<AActor>();
            UCombatAbilitySystemComponent* ASC = NewObject<UCombatAbilitySystemComponent>(Actor);
            Actor->AddInstanceComponent(ASC);
            ASC->RegisterComponent();
            ASC->InitializeCombat(Health, Invincibility);
            return ASC;
        }

        void Advance(float Seconds)
        {
            for (float Elapsed = 0.0f; Elapsed < Seconds; Elapsed += 0.05f)
            {
                ++GFrameCounter;
                World->Tick(LEVELTICK_All, 0.05f);
            }
        }
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCombatDamageTest, "TeamPotato.Combat.GAS.DamageHealingAndDeath",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCombatDamageTest::RunTest(const FString& Parameters)
{
    FCombatTestWorld Fixture;
    auto* Source = Fixture.SpawnCombatant();
    auto* Target = Fixture.SpawnCombatant();
    int32 DeathCount = 0;
    int32 HealthChanges = 0;
    bool bCorrectSource = false;
    Target->OnCombatDeath.AddLambda([&]() { ++DeathCount; });
    Target->OnCombatHealthChanged.AddLambda([&](float Health, float MaxHealth) { ++HealthChanges; });
    Target->OnCombatDamageReceived.AddLambda([&](float Damage, const FGameplayEffectContextHandle& Context)
    {
        bCorrectSource = Context.GetOriginalInstigator() == Source->GetOwner();
    });
    TestEqual(TEXT("Damage applied"), UCombatFunctionLibrary::ApplyCombatDamage(Target->GetOwner(), 30.0f, Source->GetOwner()), 30.0f);
    TestEqual(TEXT("Remaining health"), Target->GetHealth(), 70.0f);
    TestTrue(TEXT("Attacker context retained"), bCorrectSource);
    TestEqual(TEXT("Damage meta attribute consumed"), Target->GetNumericAttribute(UCombatAttributeSet::GetIncomingDamageAttribute()), 0.0f);
    Target->Heal(10.0f);
    TestEqual(TEXT("Healing"), Target->GetHealth(), 80.0f);
    Target->AddMaxHealth(25.0f);
    TestEqual(TEXT("Perk increases max HP"), Target->GetMaxHealth(), 125.0f);
    TestEqual(TEXT("Perk heals by increase"), Target->GetHealth(), 105.0f);
    Target->Heal(500.0f);
    TestEqual(TEXT("Healing clamps to max"), Target->GetHealth(), 125.0f);
    Target->AddMaxHealth(-1000.0f);
    TestEqual(TEXT("Max HP minimum"), Target->GetMaxHealth(), 1.0f);
    TestEqual(TEXT("Health follows reduced max"), Target->GetHealth(), 1.0f);
    Target->SetMaxHealthAndFill(100.0f);
    TestEqual(TEXT("Negative damage rejected"), UCombatFunctionLibrary::ApplyCombatDamage(Target->GetOwner(), -2.0f, Source->GetOwner()), 0.0f);
    TestEqual(TEXT("NaN damage rejected"), UCombatFunctionLibrary::ApplyCombatDamage(Target->GetOwner(), std::numeric_limits<float>::quiet_NaN(), Source->GetOwner()), 0.0f);
    TestEqual(TEXT("Overkill returns actual HP lost"), UCombatFunctionLibrary::ApplyCombatDamage(Target->GetOwner(), 999.0f, Source->GetOwner()), 100.0f);
    TestEqual(TEXT("Dead HP clamped"), Target->GetHealth(), 0.0f);
    TestTrue(TEXT("Dead tag granted"), Target->HasMatchingGameplayTag(CombatTags::State_Dead));
    TestEqual(TEXT("Damage after death ignored"), UCombatFunctionLibrary::ApplyCombatDamage(Target->GetOwner(), 10.0f, Source->GetOwner()), 0.0f);
    Target->Heal(100.0f);
    Target->SetMaxHealthAndFill(100.0f);
    Target->MarkDead();
    TestEqual(TEXT("Heal/reset cannot revive"), Target->GetHealth(), 0.0f);
    TestEqual(TEXT("Death notified exactly once"), DeathCount, 1);
    TestTrue(TEXT("UI health notifications delivered"), HealthChanges > 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCombatInvincibilityTest, "TeamPotato.Combat.GAS.IndependentInvincibility",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCombatInvincibilityTest::RunTest(const FString& Parameters)
{
    FCombatTestWorld Fixture;
    auto* Target = Fixture.SpawnCombatant(100.0f, 0.3f);
    TestEqual(TEXT("First hit"), UCombatFunctionLibrary::ApplyCombatDamage(Target->GetOwner(), 10.0f, nullptr), 10.0f);
    TestEqual(TEXT("Immediate second hit blocked"), UCombatFunctionLibrary::ApplyCombatDamage(Target->GetOwner(), 10.0f, nullptr), 0.0f);
    Target->BeginDodgeInvincibility();
    Target->BeginDodgeInvincibility();
    Target->EndDodgeInvincibility();
    TestTrue(TEXT("Ending dodge preserves hit invincibility"), Target->HasMatchingGameplayTag(CombatTags::State_Invincible));
    Fixture.Advance(0.5f);
    TestFalse(TEXT("Hit invincibility expires"), Target->HasMatchingGameplayTag(CombatTags::State_Invincible));
    Target->ApplyHitInvincibility();
    Target->BeginDodgeInvincibility();
    Fixture.Advance(0.5f);
    TestTrue(TEXT("Hit expiry preserves dodge"), Target->HasMatchingGameplayTag(CombatTags::State_Invincible));

    // Bypass the helper: the GE itself must reject an invulnerable target.
    FGameplayEffectSpecHandle Spec = Target->MakeOutgoingSpec(UCombatDamageEffect::StaticClass(), 1.0f, Target->MakeEffectContext());
    Spec.Data->SetSetByCallerMagnitude(CombatTags::Data_Damage, 20.0f);
    Target->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
    TestEqual(TEXT("Direct GE cannot bypass invincibility"), Target->GetHealth(), 90.0f);
    Target->EndDodgeInvincibility();
    TestFalse(TEXT("All invincibility removed"), Target->HasMatchingGameplayTag(CombatTags::State_Invincible));
    TestEqual(TEXT("Damage resumes"), UCombatFunctionLibrary::ApplyCombatDamage(Target->GetOwner(), 10.0f, nullptr), 10.0f);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCombatLegacyTest, "TeamPotato.Combat.GAS.LegacyActorsAndBossPhase",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCombatLegacyTest::RunTest(const FString& Parameters)
{
    FCombatTestWorld Fixture;
    ATestCharacter* Player = Fixture.World->SpawnActor<ATestCharacter>();
    ABossBase* Boss = Fixture.World->SpawnActor<ABossBase>();
    if (!TestNotNull(TEXT("Player spawned"), Player) || !TestNotNull(TEXT("Boss spawned"), Boss)) return false;
    auto* PlayerASC = CastChecked<UCombatAbilitySystemComponent>(Player->GetAbilitySystemComponent());
    TestEqual(TEXT("Boss initializes configured health"), Boss->GetCombatHealth(), 300.0f);
    TestEqual(TEXT("Initialization does not trigger phase 2"), Boss->CurrentPhase, 1);
    TestEqual(TEXT("Legacy ApplyDamage to player"), UGameplayStatics::ApplyDamage(Player, 20.0f, nullptr, Boss, nullptr), 20.0f);
    TestEqual(TEXT("Player damage applied once"), PlayerASC->GetHealth(), 80.0f);
    TestEqual(TEXT("Legacy ApplyDamage to boss"), UGameplayStatics::ApplyDamage(Boss, 155.0f, nullptr, Player, nullptr), 155.0f);
    TestEqual(TEXT("Boss health updated"), Boss->GetCombatHealth(), 145.0f);
    TestEqual(TEXT("Boss crosses half-health phase"), Boss->CurrentPhase, 2);
    Fixture.Advance(0.5f);
    TestEqual(TEXT("GAS hit also works on boss"), UCombatFunctionLibrary::ApplyCombatDamage(Boss, 5.0f, Player), 5.0f);
    TestEqual(TEXT("Boss Blueprint getter reads GAS"), Boss->GetCombatHealth(), 140.0f);
    AActor* LegacyActor = Fixture.World->SpawnActor<AActor>();
    TestEqual(TEXT("Non-GAS damage fallback"), UCombatFunctionLibrary::ApplyCombatDamage(LegacyActor, 12.0f, Player), 12.0f);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCombatHitSourceTest, "TeamPotato.Combat.GAS.MeleeDeduplicationAndReusedSource",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCombatHitSourceTest::RunTest(const FString& Parameters)
{
    FCombatTestWorld Fixture;
    auto* Source = Fixture.SpawnCombatant();
    auto* OtherSource = Fixture.SpawnCombatant();
    auto* Target = Fixture.SpawnCombatant();
    ASwordWeaponActor* Sword = Fixture.World->SpawnActor<ASwordWeaponActor>();
    Sword->SetOwner(Source->GetOwner());
    UWeaponDataAsset* Data = NewObject<UWeaponDataAsset>(Sword);
    Data->AttackDamage = 10.0f;
    Sword->InitializeFromData(Data);
    Sword->BeginAttack();
    Sword->DamageToTarget(Target->GetOwner());
    Sword->DamageToTarget(Target->GetOwner());
    TestEqual(TEXT("One swing hits a target once even without invincibility"), Target->GetHealth(), 90.0f);
    Sword->EndAttack();
    Sword->BeginAttack();
    Sword->DamageToTarget(Target->GetOwner());
    TestEqual(TEXT("Next swing can hit again"), Target->GetHealth(), 80.0f);
    Sword->EndAttack();

    AActor* ReusedProjectile = Fixture.World->SpawnActor<AActor>();
    AActor* LastInstigator = nullptr;
    AActor* LastCauser = nullptr;
    bool bHasHitResult = false;
    Target->OnCombatDamageReceived.AddLambda([&](float Damage, const FGameplayEffectContextHandle& Context)
    {
        LastInstigator = Context.GetOriginalInstigator();
        LastCauser = Context.GetEffectCauser();
        bHasHitResult = Context.GetHitResult() != nullptr;
    });
    ReusedProjectile->SetOwner(Source->GetOwner());
    UCombatFunctionLibrary::ApplyCombatDamage(Target->GetOwner(), 5.0f, ReusedProjectile);
    TestTrue(TEXT("First projectile owner resolved"), LastInstigator == Source->GetOwner());
    ReusedProjectile->SetOwner(OtherSource->GetOwner());
    FHitResult Hit;
    Hit.ImpactPoint = FVector(100.0f, 0.0f, 0.0f);
    UCombatFunctionLibrary::ApplyCombatDamageWithHit(Target->GetOwner(), 7.0f, ReusedProjectile, nullptr, &Hit);
    TestTrue(TEXT("Reused projectile resolves new owner"), LastInstigator == OtherSource->GetOwner());
    TestTrue(TEXT("Projectile preserved as effect causer"), LastCauser == ReusedProjectile);
    TestTrue(TEXT("Hit result preserved"), bHasHitResult);
    TestEqual(TEXT("Each hit uses its current damage"), Target->GetHealth(), 68.0f);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCombatEffectSelectionTest, "TeamPotato.Combat.GAS.ConfigurableEffectClasses",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCombatEffectSelectionTest::RunTest(const FString& Parameters)
{
    FCombatTestWorld Fixture;
    auto* Source = Fixture.SpawnCombatant();
    auto* Target = Fixture.SpawnCombatant();
    // An empty GE distinguishes the selected class from the native damage/heal defaults.
    Source->DamageEffectClass = UGameplayEffect::StaticClass();
    TestEqual(TEXT("Attacker-selected GE replaces native damage"),
        UCombatFunctionLibrary::ApplyCombatDamage(Target->GetOwner(), 10.0f, Source->GetOwner()), 0.0f);
    Source->DamageEffectClass = nullptr;
    TestEqual(TEXT("Empty damage slot safely disables damage"),
        UCombatFunctionLibrary::ApplyCombatDamage(Target->GetOwner(), 10.0f, Source->GetOwner()), 0.0f);
    Source->DamageEffectClass = UCombatDamageEffect::StaticClass();
    Target->DamageEffectClass = UGameplayEffect::StaticClass();
    TestEqual(TEXT("Attacker class takes precedence over target class"),
        UCombatFunctionLibrary::ApplyCombatDamage(Target->GetOwner(), 10.0f, Source->GetOwner()), 10.0f);
    TestEqual(TEXT("World hazard uses target-selected damage class"),
        UCombatFunctionLibrary::ApplyCombatDamage(Target->GetOwner(), 10.0f, nullptr), 0.0f);

    Target->HealEffectClass = UGameplayEffect::StaticClass();
    Target->Heal(5.0f);
    TestEqual(TEXT("Selected heal GE is used"), Target->GetHealth(), 90.0f);
    Target->MaxHealthEffectClass = UGameplayEffect::StaticClass();
    Target->AddMaxHealth(25.0f);
    TestEqual(TEXT("Selected max-health GE is used"), Target->GetMaxHealth(), 100.0f);
    Target->HealEffectClass = nullptr;
    Target->MaxHealthEffectClass = nullptr;
    Target->Heal(5.0f);
    Target->AddMaxHealth(25.0f);
    TestEqual(TEXT("Empty magnitude slots are safe"), Target->GetHealth(), 90.0f);

    int32 SelectedEffectApplications = 0;
    Target->OnGameplayEffectAppliedDelegateToSelf.AddLambda(
        [&](UAbilitySystemComponent*, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle)
        {
            if (Spec.Def && Spec.Def->GetClass() == UGameplayEffect::StaticClass()) ++SelectedEffectApplications;
        });
    Target->bOverrideHitInvincibilityDuration = false;
    Target->HitInvincibilityEffectClass = UGameplayEffect::StaticClass();
    Target->DodgeInvincibilityEffectClass = UGameplayEffect::StaticClass();
    Target->DeadEffectClass = UGameplayEffect::StaticClass();
    Target->ApplyHitInvincibility();
    Target->BeginDodgeInvincibility();
    Target->EndDodgeInvincibility();
    Target->MarkDead();
    TestEqual(TEXT("Hit, dodge and death all apply the configured class"), SelectedEffectApplications, 3);
    TestTrue(TEXT("Death handling still runs with a custom GE"), Target->IsDead());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCombatEffectDurationTest, "TeamPotato.Combat.GAS.EffectAuthoredInvincibilityDuration",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCombatEffectDurationTest::RunTest(const FString& Parameters)
{
    FCombatTestWorld Fixture;
    auto* Target = Fixture.SpawnCombatant(100.0f, 0.1f);
    Target->bOverrideHitInvincibilityDuration = false;
    Target->ApplyHitInvincibility();
    Fixture.Advance(0.3f);
    TestTrue(TEXT("GE duration is not overwritten by character duration"), Target->HasMatchingGameplayTag(CombatTags::State_Invincible));
    Fixture.Advance(0.9f);
    TestFalse(TEXT("GE-authored one-second duration expires"), Target->HasMatchingGameplayTag(CombatTags::State_Invincible));
    Target->bOverrideHitInvincibilityDuration = true;
    Target->ApplyHitInvincibility();
    Fixture.Advance(0.3f);
    TestFalse(TEXT("Character duration override remains supported"), Target->HasMatchingGameplayTag(CombatTags::State_Invincible));
    return true;
}

#endif
