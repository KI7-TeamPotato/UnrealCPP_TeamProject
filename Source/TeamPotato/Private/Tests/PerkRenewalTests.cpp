#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Combat/CombatAbilitySystemComponent.h"
#include "Combat/CombatAttributeSet.h"
#include "Combat/CombatEffects.h"
#include "Combat/CombatFunctionLibrary.h"
#include "Combat/CombatTags.h"
#include "Component/PerkComponent.h"
#include "Component/PlayerResource.h"
#include "Data/PerkDataAsset.h"
#include "Data/PerkDataTableRow.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameplayEffect.h"
#include "Player/Perk/GameplayEffectPerkEffect.h"
#include "Player/TestCharacter.h"

namespace
{
    UPerkDataAsset* LoadRenewedPerk(const TCHAR* Suffix)
    {
        const FString Path = FString::Printf(TEXT("/Game/UI/Data/Perk/DataAsset/Renewed/DA_Perk_%s"), Suffix);
        return LoadObject<UPerkDataAsset>(nullptr, *Path);
    }

    UCombatStatusDefinition* MakeRenewalStatus(ECombatStatus Type)
    {
        auto* Definition = NewObject<UCombatStatusDefinition>();
        Definition->Status = Type;
        Definition->DamagePerTick = 5.0f;
        if (Type == ECombatStatus::Poison)
        {
            Definition->DamageEffect = UCombatPoisonEffect::StaticClass();
            Definition->PoisonSlowEffect = UCombatPoisonSlowEffect::StaticClass();
        }
        else if (Type == ECombatStatus::Fire) Definition->DamageEffect = UCombatFireEffect::StaticClass();
        else Definition->DamageEffect = UCombatElectricEffect::StaticClass();
        return Definition;
    }

    struct FPerkRenewalWorld
    {
        UWorld* World;
        ATestCharacter* Player;
        UCombatAbilitySystemComponent* ASC;
        UPerkComponent* Perks;
        UPlayerResource* Resource;
        uint64 InitialFrame = GFrameCounter;

        FPerkRenewalWorld()
        {
            World = UWorld::CreateWorld(EWorldType::GamePreview, false);
            GEngine->CreateNewWorldContext(EWorldType::GamePreview).SetCurrentWorld(World);
            World->InitializeActorsForPlay(FURL());
            Player = World->SpawnActor<ATestCharacter>();
            ASC = CastChecked<UCombatAbilitySystemComponent>(Player->GetAbilitySystemComponent());
            Perks = Player->FindComponentByClass<UPerkComponent>();
            Resource = Player->GetResource();
            ASC->HitInvincibilityEffectClass = nullptr;
        }

        ~FPerkRenewalWorld()
        {
            Perks->ClearAllEquippedPerks();
            ASC->RemoveAllStatuses();
            World->DestroyWorld(false);
            GEngine->DestroyWorldContext(World);
            GFrameCounter = InitialFrame;
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRenewedPerkAssetsTest, "TeamPotato.Perks.Renewal.AssetTable",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRenewedPerkAssetsTest::RunTest(const FString& Parameters)
{
    auto* Table = LoadObject<UDataTable>(nullptr, TEXT("/Game/UI/Data/Perk/DataTable/DT_SelectPerkData"));
    if (!TestNotNull(TEXT("The live perk selection table loads"), Table)) return false;
    TArray<FPerkSelectDataTableRow*> Rows;
    Table->GetAllRows(TEXT("PerkRenewal"), Rows);
    TestEqual(TEXT("The old pool is replaced by exactly eight perks"), Rows.Num(), 8);
    TSet<UPerkDataAsset*> UniquePerks;
    TSet<UClass*> UniqueEffects;
    for (const FPerkSelectDataTableRow* Row : Rows)
    {
        if (!TestNotNull(TEXT("Every row has a data asset"), Row->PerkDataAsset.Get())) continue;
        UPerkDataAsset* Perk = Row->PerkDataAsset;
        UniquePerks.Add(Perk);
        TestTrue(TEXT("Every new perk can be selected"), FMath::IsFinite(Row->SelectWeight) && Row->SelectWeight > 0.0f);
        TestFalse(TEXT("Every perk has a name"), Perk->PerkName.IsEmpty());
        TestFalse(TEXT("Every perk has a formatted description"), Perk->GetFormattedDescription().IsEmpty());
        TestNotNull(TEXT("Every perk has an icon"), Perk->Icon);
        const auto* Logic = Cast<UGameplayEffectPerkEffect>(Perk->PerkEffect);
        if (!TestNotNull(TEXT("Every new perk uses a GAS effect"), Logic)) continue;
        const UGameplayEffect* Effect = Logic->EffectClass.GetDefaultObject();
        if (!TestNotNull(TEXT("Every perk GE loads"), Effect)) continue;
        UniqueEffects.Add(Effect->GetClass());
        TestTrue(TEXT("Perks remain active until removed"), Effect->DurationPolicy == EGameplayEffectDurationType::Infinite);
        TestEqual(TEXT("Perk modifiers are not periodic"), Effect->Period.GetValueAtLevel(1.0f), 0.0f);
    }
    TestEqual(TEXT("The eight rows use distinct perk data assets"), UniquePerks.Num(), 8);
    TestEqual(TEXT("Each perk owns a distinct GE"), UniqueEffects.Num(), 8);
    for (const TCHAR* Suffix : {TEXT("PoisonImmunity"), TEXT("ElectricImmunity"), TEXT("FireImmunity"), TEXT("MaxHealth"),
        TEXT("GoldGain"), TEXT("Cooldown"), TEXT("MaxEnergy"), TEXT("DamageReduction")})
    {
        UPerkDataAsset* Perk = LoadRenewedPerk(Suffix);
        TestTrue(FString::Printf(TEXT("The selection pool contains %s"), Suffix), Perk && UniquePerks.Contains(Perk));
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRenewedPerkImmunityTest, "TeamPotato.Perks.Renewal.ElementalImmunities",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRenewedPerkImmunityTest::RunTest(const FString& Parameters)
{
    const TCHAR* Names[] = {TEXT("PoisonImmunity"), TEXT("ElectricImmunity"), TEXT("FireImmunity")};
    const ECombatStatus Types[] = {ECombatStatus::Poison, ECombatStatus::Electric, ECombatStatus::Fire};
    for (int32 Index = 0; Index < UE_ARRAY_COUNT(Names); ++Index)
    {
        FPerkRenewalWorld Fixture;
        UPerkDataAsset* Perk = LoadRenewedPerk(Names[Index]);
        if (!TestNotNull(Names[Index], Perk)) return false;
        const ECombatStatus Type = Types[Index];
        UCombatStatusDefinition* Status = MakeRenewalStatus(Type);
        TestTrue(TEXT("The status applies before acquiring immunity"), Fixture.ASC->ApplyStatus(Status, Fixture.ASC->MakeEffectContext()));
        TestTrue(TEXT("The original DOT is active"), Fixture.ASC->HasStatus(Type));
        if (Type == ECombatStatus::Poison)
            TestTrue(TEXT("Poison starts with its movement penalty"), Fixture.ASC->GetNumericAttribute(UCombatAttributeSet::GetMoveSpeedMultiplierAttribute()) < 1.0f);
        if (Type == ECombatStatus::Electric) TestFalse(TEXT("Electric initially prevents attacks"), Fixture.ASC->CanAttack());
        if (!TestTrue(TEXT("The immunity perk equips through the real component"), Fixture.Perks->EquipPerk(Perk))) return false;
        TestTrue(TEXT("Equipping grants the corresponding immunity"), Fixture.ASC->IsImmuneToStatus(Type));
        TestFalse(TEXT("Acquiring immunity immediately purges the active DOT"), Fixture.ASC->HasStatus(Type));
        TestEqual(TEXT("Poison slow is not left behind"), Fixture.ASC->GetNumericAttribute(UCombatAttributeSet::GetMoveSpeedMultiplierAttribute()), 1.0f);
        TestTrue(TEXT("Electric attack blocking is not left behind"), Fixture.ASC->CanAttack());
        TestFalse(TEXT("The immune status cannot be reapplied"), Fixture.ASC->ApplyStatus(Status, Fixture.ASC->MakeEffectContext()));
        TestFalse(TEXT("An owned perk cannot be selected twice"), Fixture.Perks->EquipPerk(Perk));
        Fixture.Advance(1.1f);
        TestEqual(TEXT("Purged elemental damage no longer ticks"), Fixture.ASC->GetHealth(), 100.0f);
        const ECombatStatus OtherType = Type == ECombatStatus::Fire ? ECombatStatus::Poison : ECombatStatus::Fire;
        TestTrue(TEXT("Immunity leaves other elements vulnerable"), Fixture.ASC->ApplyStatus(MakeRenewalStatus(OtherType), Fixture.ASC->MakeEffectContext()));
        Fixture.Advance(1.1f);
        TestEqual(TEXT("A different element still deals damage"), Fixture.ASC->GetHealth(), 95.0f);
        Fixture.ASC->RemoveAllStatuses();
        Fixture.Perks->ClearAllEquippedPerks();
        TestFalse(TEXT("Clearing perks removes immunity"), Fixture.ASC->IsImmuneToStatus(Type));
        TestTrue(TEXT("The original status applies again after removal"), Fixture.ASC->ApplyStatus(Status, Fixture.ASC->MakeEffectContext()));
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRenewedPerkCapacityTest, "TeamPotato.Perks.Renewal.HealthAndEnergy",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRenewedPerkCapacityTest::RunTest(const FString& Parameters)
{
    FPerkRenewalWorld Fixture;
    UPerkDataAsset* Health = LoadRenewedPerk(TEXT("MaxHealth"));
    UPerkDataAsset* Energy = LoadRenewedPerk(TEXT("MaxEnergy"));
    if (!TestNotNull(TEXT("Health perk loads"), Health) || !TestNotNull(TEXT("Energy perk loads"), Energy)) return false;
    Fixture.ASC->SetNumericAttributeBase(UCombatAttributeSet::GetHealthAttribute(), 80.0f);
    Fixture.Resource->UseEnergy(40.0f);
    if (!TestTrue(TEXT("The health perk equips"), Fixture.Perks->EquipPerk(Health)) ||
        !TestTrue(TEXT("The energy perk equips"), Fixture.Perks->EquipPerk(Energy))) return false;
    // GAS uses float magnitudes: 100 * 1.2f can be 120.0000076, above IsNearlyEqual's default 1e-8 tolerance.
    AddInfo(FString::Printf(TEXT("Health after the authored perk: max=%.9f current=%.9f"), Fixture.ASC->GetMaxHealth(), Fixture.ASC->GetHealth()));
    TestEqual(TEXT("Maximum health increases from one hundred to one hundred twenty"), Fixture.ASC->GetMaxHealth(), 120.0f, 0.001f);
    TestEqual(TEXT("Health only fills the twenty new points"), Fixture.ASC->GetHealth(), 100.0f, 0.001f);
    TestEqual(TEXT("Maximum energy increases by eighty"), Fixture.Resource->GetMaxEnergyAmount(), 180.0f);
    TestEqual(TEXT("Energy only fills the eighty new points"), Fixture.Resource->GetEnergyAmount(), 140.0f);
    TestFalse(TEXT("Health cannot be stacked by selecting the same perk again"), Fixture.Perks->EquipPerk(Health));
    TestEqual(TEXT("Duplicate rejection leaves the maximum unchanged"), Fixture.ASC->GetMaxHealth(), 120.0f, 0.001f);
    Fixture.ASC->Heal(1000.0f);
    Fixture.Resource->FillEnergy(1000.0f);
    Fixture.Perks->ClearAllEquippedPerks();
    TestEqual(TEXT("Clearing perks restores the health cap"), Fixture.ASC->GetMaxHealth(), 100.0f);
    TestEqual(TEXT("Health is clamped when its capacity decreases"), Fixture.ASC->GetHealth(), 100.0f);
    TestEqual(TEXT("Clearing perks restores the energy cap"), Fixture.Resource->GetMaxEnergyAmount(), 100.0f);
    TestEqual(TEXT("Energy is clamped when its capacity decreases"), Fixture.Resource->GetEnergyAmount(), 100.0f);
    for (UPerkDataAsset* Equipped : Fixture.Perks->GetEquippedPerks()) TestNull(TEXT("Every perk slot is empty after clearing"), Equipped);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRenewedPerkEconomyCooldownTest, "TeamPotato.Perks.Renewal.GoldAndCooldown",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRenewedPerkEconomyCooldownTest::RunTest(const FString& Parameters)
{
    FPerkRenewalWorld Fixture;
    UPerkDataAsset* Gold = LoadRenewedPerk(TEXT("GoldGain"));
    UPerkDataAsset* Cooldown = LoadRenewedPerk(TEXT("Cooldown"));
    if (!TestNotNull(TEXT("Gold perk loads"), Gold) || !TestNotNull(TEXT("Cooldown perk loads"), Cooldown)) return false;
    if (!TestTrue(TEXT("Gold perk equips"), Fixture.Perks->EquipPerk(Gold)) ||
        !TestTrue(TEXT("Cooldown perk equips"), Fixture.Perks->EquipPerk(Cooldown))) return false;
    Fixture.Resource->AddGold(100);
    TestEqual(TEXT("The authored gold perk increases rewards by thirty percent"), Fixture.Resource->GetCurrentGold(), 130);
    Fixture.Resource->AddGold(-30);
    TestEqual(TEXT("The authored gold perk does not change spending"), Fixture.Resource->GetCurrentGold(), 100);
    TestEqual(TEXT("The authored cooldown perk halves cooldown duration"),
        Fixture.ASC->GetNumericAttribute(UCombatAttributeSet::GetCooldownDurationMultiplierAttribute()), 0.5f);
    Fixture.Perks->ClearAllEquippedPerks();
    TestEqual(TEXT("Clearing restores the normal cooldown multiplier"),
        Fixture.ASC->GetNumericAttribute(UCombatAttributeSet::GetCooldownDurationMultiplierAttribute()), 1.0f);
    Fixture.Resource->AddGold(100);
    TestEqual(TEXT("Clearing restores normal gold rewards"), Fixture.Resource->GetCurrentGold(), 200);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRenewedPerkDamageReductionTest, "TeamPotato.Perks.Renewal.DamageReduction",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRenewedPerkDamageReductionTest::RunTest(const FString& Parameters)
{
    FPerkRenewalWorld Fixture;
    Fixture.ASC->SetMaxHealthAndFill(1000.0f);
    UPerkDataAsset* Reduction = LoadRenewedPerk(TEXT("DamageReduction"));
    if (!TestNotNull(TEXT("Damage reduction perk loads"), Reduction) ||
        !TestTrue(TEXT("Damage reduction perk equips"), Fixture.Perks->EquipPerk(Reduction))) return false;
    TestTrue(TEXT("One hundred incoming damage becomes eighty"), FMath::IsNearlyEqual(
        UCombatFunctionLibrary::ApplyCombatDamage(Fixture.Player, 100.0f, nullptr), 80.0f));
    Fixture.ASC->AddLooseGameplayTag(CombatTags::State_DamageReduction);
    TestTrue(TEXT("Knight's thirty percent reduction multiplies with the twenty percent perk"), FMath::IsNearlyEqual(
        UCombatFunctionLibrary::ApplyCombatDamage(Fixture.Player, 100.0f, nullptr), 56.0f));
    Fixture.ASC->RemoveLooseGameplayTag(CombatTags::State_DamageReduction);
    const float BeforePeriodic = Fixture.ASC->GetHealth();
    TestTrue(TEXT("The test DOT applies"), Fixture.ASC->ApplyStatus(MakeRenewalStatus(ECombatStatus::Fire), Fixture.ASC->MakeEffectContext()));
    Fixture.Advance(1.1f);
    TestTrue(TEXT("Periodic damage also receives the twenty percent reduction"), FMath::IsNearlyEqual(BeforePeriodic - Fixture.ASC->GetHealth(), 4.0f));
    Fixture.ASC->RemoveAllStatuses();
    Fixture.Perks->ClearAllEquippedPerks();
    TestTrue(TEXT("Removing the perk restores normal incoming damage"), FMath::IsNearlyEqual(
        UCombatFunctionLibrary::ApplyCombatDamage(Fixture.Player, 100.0f, nullptr), 100.0f));
    return true;
}

#endif
