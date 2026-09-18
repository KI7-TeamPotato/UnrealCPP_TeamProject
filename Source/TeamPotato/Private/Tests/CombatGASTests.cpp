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
#include "GameFramework/CharacterMovementComponent.h"
#include <limits>

namespace
{
    UCombatStatusDefinition* MakeStatus(ECombatStatus Type, float Damage = 5.0f)
    {
        auto* Definition = NewObject<UCombatStatusDefinition>();
        Definition->Status = Type;
        Definition->DamagePerTick = Damage;
        if (Type == ECombatStatus::Poison)
        {
            Definition->DamageEffect = UCombatPoisonEffect::StaticClass();
            Definition->PoisonSlowEffect = UCombatPoisonSlowEffect::StaticClass();
        }
        else if (Type == ECombatStatus::Fire) Definition->DamageEffect = UCombatFireEffect::StaticClass();
        else Definition->DamageEffect = UCombatElectricEffect::StaticClass();
        return Definition;
    }

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

        UCombatAbilitySystemComponent* SpawnPlayerCombatant(float Health = 100.0f)
        {
            ATestCharacter* Player = World->SpawnActor<ATestCharacter>();
            auto* ASC = CastChecked<UCombatAbilitySystemComponent>(Player->GetAbilitySystemComponent());
            ASC->SetMaxHealthAndFill(Health);
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCombatElementsTest, "TeamPotato.Combat.GAS.Status.CoexistenceAndInvincibility",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCombatElementsTest::RunTest(const FString& Parameters)
{
    FCombatTestWorld Fixture;
    auto* Source = Fixture.SpawnCombatant();
    auto* Target = Fixture.SpawnPlayerCombatant(1000.0f);
    int32 NormalHits = 0;
    int32 Ticks = 0;
    Target->OnCombatDamageReceived.AddLambda([&](float, const FGameplayEffectContextHandle&) { ++NormalHits; });
    Target->OnCombatPeriodicDamageReceived.AddLambda([&](float, const FGameplayEffectContextHandle& Context)
    {
        ++Ticks;
        TestTrue(TEXT("DOT retains attacker"), Context.GetOriginalInstigator() == Source->GetOwner());
    });
    for (ECombatStatus Type : {ECombatStatus::Poison, ECombatStatus::Fire, ECombatStatus::Electric})
        TestTrue(TEXT("Status applies"), UCombatFunctionLibrary::ApplyCombatStatus(Target->GetOwner(), MakeStatus(Type), Source->GetOwner()));
    TestEqual(TEXT("No immediate tick"), Target->GetHealth(), 1000.0f);
    TestFalse(TEXT("Electric blocks attacks"), Target->CanAttack());
    Target->BeginDodgeInvincibility();
    Fixture.Advance(1.2f);
    TestEqual(TEXT("All three tick during dodge"), Target->GetHealth(), 985.0f);
    TestEqual(TEXT("No normal hit reactions from DOT"), NormalHits, 0);
    TestEqual(TEXT("One tick per element"), Ticks, 3);
    Target->EndDodgeInvincibility();
    TestFalse(TEXT("DOT does not grant hit invincibility"), Target->HasMatchingGameplayTag(CombatTags::State_Invincible));
    Target->RemoveAllStatuses();
    TestTrue(TEXT("Cleansing restores attack"), Target->CanAttack());
    TestFalse(TEXT("Cleansing removes poison"), Target->HasMatchingGameplayTag(CombatTags::State_Poison));
    Fixture.Advance(1.2f);
    TestEqual(TEXT("No damage after cleanse"), Target->GetHealth(), 985.0f);
    Target->BeginDodgeInvincibility();
    TestFalse(TEXT("Dodge blocks new standalone status"), UCombatFunctionLibrary::ApplyCombatStatus(Target->GetOwner(), MakeStatus(ECombatStatus::Fire), Source->GetOwner()));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCombatStatusRefreshTest, "TeamPotato.Combat.GAS.Status.RefreshAndExpiry",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCombatStatusRefreshTest::RunTest(const FString& Parameters)
{
    FCombatTestWorld Fixture;
    auto* Source = Fixture.SpawnCombatant();
    auto* OtherSource = Fixture.SpawnCombatant();
    auto* Target = Fixture.SpawnPlayerCombatant(1000.0f);
    AActor* LastSource = nullptr;
    Target->OnCombatPeriodicDamageReceived.AddLambda([&](float, const FGameplayEffectContextHandle& Context) { LastSource = Context.GetOriginalInstigator(); });
    UCombatFunctionLibrary::ApplyCombatStatus(Target->GetOwner(), MakeStatus(ECombatStatus::Poison), Source->GetOwner());
    Fixture.Advance(0.7f);
    TestTrue(TEXT("Same type refresh succeeds"), UCombatFunctionLibrary::ApplyCombatStatus(Target->GetOwner(), MakeStatus(ECombatStatus::Poison, 9.0f), OtherSource->GetOwner()));
    Fixture.Advance(0.45f);
    TestEqual(TEXT("Refresh preserves next tick, replaces magnitude, does not stack"), Target->GetHealth(), 991.0f);
    TestTrue(TEXT("Latest source owns refreshed DOT"), LastSource == OtherSource->GetOwner());
    TestEqual(TEXT("Slow is not multiplied twice"), Target->GetNumericAttribute(UCombatAttributeSet::GetMoveSpeedMultiplierAttribute()), 0.7f);
    Fixture.Advance(4.1f);
    TestTrue(TEXT("Refresh extends lifetime beyond original duration"), Target->HasStatus(ECombatStatus::Poison));
    Fixture.Advance(0.8f);
    TestFalse(TEXT("DOT expires"), Target->HasStatus(ECombatStatus::Poison));
    TestEqual(TEXT("Poison expiry removes linked slow"), Target->GetNumericAttribute(UCombatAttributeSet::GetMoveSpeedMultiplierAttribute()), 1.0f);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCombatStatusMovementTest, "TeamPotato.Combat.GAS.Status.MovementAndAttackGate",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCombatStatusMovementTest::RunTest(const FString& Parameters)
{
    FCombatTestWorld Fixture;
    auto* Player = Fixture.World->SpawnActor<ATestCharacter>();
    auto* ASC = CastChecked<UCombatAbilitySystemComponent>(Player->GetAbilitySystemComponent());
    auto* Target = Fixture.SpawnCombatant();
    ASC->SetBaseMoveSpeed(600.0f);
    UCombatFunctionLibrary::ApplyCombatStatus(Player, MakeStatus(ECombatStatus::Poison), Target->GetOwner());
    TestEqual(TEXT("Poison affects actual movement"), Player->GetCharacterMovement()->MaxWalkSpeed, 420.0f);
    Player->AddMoveSpeed(200.0f);
    TestEqual(TEXT("Base speed upgrades survive poison"), Player->GetCharacterMovement()->MaxWalkSpeed, 560.0f);
    ASC->RemoveStatus(ECombatStatus::Poison);
    TestEqual(TEXT("Cleanse restores upgraded speed"), Player->GetCharacterMovement()->MaxWalkSpeed, 800.0f);

    auto* Sword = Fixture.World->SpawnActor<ASwordWeaponActor>();
    Sword->SetOwner(Player);
    auto* WeaponData = NewObject<UWeaponDataAsset>();
    WeaponData->AttackDamage = 10.0f;
    Sword->InitializeFromData(WeaponData);
    Sword->BeginAttack();
    UCombatFunctionLibrary::ApplyCombatStatus(Player, MakeStatus(ECombatStatus::Electric), Target->GetOwner());
    TestFalse(TEXT("Public attack gate rejects shocked player"), UCombatFunctionLibrary::CanActorAttack(Player));
    TestEqual(TEXT("Character-source damage cannot bypass shock"), UCombatFunctionLibrary::ApplyCombatDamage(Target->GetOwner(), 10.0f, Player), 0.0f);
    Sword->AttachToActor(Player, FAttachmentTransformRules::KeepWorldTransform);
    TestEqual(TEXT("Attached weapon damage cannot bypass shock"), UCombatFunctionLibrary::ApplyCombatDamage(Target->GetOwner(), 10.0f, Sword), 0.0f);
    Sword->DamageToTarget(Target->GetOwner());
    TestEqual(TEXT("An already-open melee window cannot damage during shock"), Target->GetHealth(), 100.0f);
    TestEqual(TEXT("Electric does not slow movement"), Player->GetCharacterMovement()->MaxWalkSpeed, 800.0f);
    auto* Projectile = Fixture.World->SpawnActor<AActor>();
    Projectile->SetOwner(Player);
    TestEqual(TEXT("Already-fired projectile still hits while owner is shocked"), UCombatFunctionLibrary::ApplyCombatDamage(Target->GetOwner(), 7.0f, Projectile), 7.0f);
    ASC->RemoveStatus(ECombatStatus::Electric);
    Sword->BeginAttack();
    Sword->DamageToTarget(Target->GetOwner());
    TestEqual(TEXT("Attacks resume after shock cleanse"), Target->GetHealth(), 83.0f);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCombatStatusDeathTest, "TeamPotato.Combat.GAS.Status.OnHitDeathAndValidation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCombatStatusDeathTest::RunTest(const FString& Parameters)
{
    FCombatTestWorld Fixture;
    auto* Source = Fixture.SpawnCombatant();
    auto* Target = Fixture.SpawnPlayerCombatant(20.0f);
    TArray<UCombatStatusDefinition*> Statuses = {MakeStatus(ECombatStatus::Poison, 15.0f), MakeStatus(ECombatStatus::Electric, 15.0f)};
    int32 DeathCount = 0;
    Target->OnCombatDeath.AddLambda([&]() { ++DeathCount; });
    TestEqual(TEXT("Combined hit damages"), UCombatFunctionLibrary::ApplyCombatDamageWithStatuses(Target->GetOwner(), 5.0f, Source->GetOwner(), Statuses), 5.0f);
    TestTrue(TEXT("On-hit status not blocked by invincibility caused by its own hit"), Target->HasStatus(ECombatStatus::Poison));
    Target->BeginDodgeInvincibility();
    Fixture.Advance(1.2f);
    TestTrue(TEXT("DOT can kill through existing hit invincibility"), Target->IsDead());
    TestEqual(TEXT("Death once"), DeathCount, 1);
    TestFalse(TEXT("Death removes statuses"), Target->HasStatus(ECombatStatus::Poison));
    TestFalse(TEXT("Death removes attack block tag"), Target->HasMatchingGameplayTag(CombatTags::State_AttackBlocked));
    TestEqual(TEXT("Death removes slow"), Target->GetNumericAttribute(UCombatAttributeSet::GetMoveSpeedMultiplierAttribute()), 1.0f);
    TestFalse(TEXT("Death still blocks attacks"), Target->CanAttack());
    Fixture.Advance(1.2f);
    TestEqual(TEXT("No repeated deaths"), DeathCount, 1);

    auto* Invalid = MakeStatus(ECombatStatus::Fire);
    Invalid->DamageEffect = UCombatDamageEffect::StaticClass();
    auto* LivingPlayer = Fixture.SpawnPlayerCombatant();
    AddExpectedError(TEXT("Invalid status DOT configuration"), EAutomationExpectedErrorFlags::Contains, 1);
    TestFalse(TEXT("Invalid instant status GE rejected"), UCombatFunctionLibrary::ApplyCombatStatus(LivingPlayer->GetOwner(), Invalid, nullptr));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCombatPlayerOnlyStatusTest, "TeamPotato.Combat.GAS.Status.PlayerOnlyTargets",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCombatPlayerOnlyStatusTest::RunTest(const FString& Parameters)
{
    FCombatTestWorld Fixture;
    auto* PlayerASC = Fixture.SpawnPlayerCombatant(1000.0f);
    auto* Enemy = Fixture.World->SpawnActor<AEnemyCharacter>();
    auto* Boss = Fixture.World->SpawnActor<ABossBase>();
    auto* GenericASC = Fixture.SpawnCombatant();
    auto* EnemyASC = CastChecked<UCombatAbilitySystemComponent>(Enemy->GetAbilitySystemComponent());
    auto* BossASC = CastChecked<UCombatAbilitySystemComponent>(Boss->GetAbilitySystemComponent());
    Enemy->UpdateMovementSpeed(400.0f);
    Boss->UpdateMovementSpeed(500.0f);
    const float EnemyHealth = EnemyASC->GetHealth();
    const float BossHealth = BossASC->GetHealth();
    TestTrue(TEXT("Unpossessed player class is eligible"), PlayerASC->CanReceiveStatuses());
    for (ECombatStatus Type : {ECombatStatus::Poison, ECombatStatus::Fire, ECombatStatus::Electric})
    {
        auto* Definition = MakeStatus(Type);
        TestTrue(TEXT("Enemy can inflict status on player"), UCombatFunctionLibrary::ApplyCombatStatus(PlayerASC->GetOwner(), Definition, Enemy));
        for (auto* ASC : {EnemyASC, BossASC, GenericASC})
        {
            TestFalse(TEXT("Non-player is not eligible"), ASC->CanReceiveStatuses());
            TestFalse(TEXT("Public node rejects non-player"), UCombatFunctionLibrary::ApplyCombatStatus(ASC->GetOwner(), Definition, PlayerASC->GetOwner()));
            TestFalse(TEXT("Direct ASC call also rejects non-player"), ASC->ApplyStatus(Definition, PlayerASC->MakeEffectContext()));
            auto Spec = PlayerASC->MakeOutgoingSpec(Definition->DamageEffect, 1.0f, PlayerASC->MakeEffectContext());
            Spec.Data->SetSetByCallerMagnitude(CombatTags::Data_Damage, 10.0f);
            TestFalse(TEXT("Native GE template also requires player tag"), ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get()).IsValid());
        }
    }
    Fixture.Advance(1.2f);
    TestEqual(TEXT("Player receives all three DOTs"), PlayerASC->GetHealth(), 985.0f);
    TestEqual(TEXT("Enemy takes no status damage"), EnemyASC->GetHealth(), EnemyHealth);
    TestEqual(TEXT("Boss takes no status damage"), BossASC->GetHealth(), BossHealth);
    TestEqual(TEXT("Enemy speed untouched"), Enemy->GetCharacterMovement()->MaxWalkSpeed, 400.0f);
    TestEqual(TEXT("Boss speed untouched"), Boss->GetCharacterMovement()->MaxWalkSpeed, 500.0f);
    TestTrue(TEXT("Enemy attack unaffected"), EnemyASC->CanAttack());
    TestTrue(TEXT("Boss attack unaffected"), BossASC->CanAttack());
    PlayerASC->RemoveAllStatuses();
    const TArray<UCombatStatusDefinition*> Statuses = {MakeStatus(ECombatStatus::Poison)};
    TestEqual(TEXT("Combined hit retains ordinary damage to enemy"), UCombatFunctionLibrary::ApplyCombatDamageWithStatuses(Enemy, 5.0f, PlayerASC->GetOwner(), Statuses), 5.0f);
    TestFalse(TEXT("Combined hit does not inflict poison on enemy"), EnemyASC->HasStatus(ECombatStatus::Poison));
    TestEqual(TEXT("Legacy boss damage unchanged"), UGameplayStatics::ApplyDamage(Boss, 5.0f, nullptr, PlayerASC->GetOwner(), nullptr), 5.0f);
    return true;
}

#endif
