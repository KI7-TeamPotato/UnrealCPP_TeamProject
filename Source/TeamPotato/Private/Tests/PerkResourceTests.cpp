#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Combat/CombatAbilitySystemComponent.h"
#include "Combat/CombatAttributeSet.h"
#include "Component/PlayerResource.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameplayEffect.h"
#include "Player/TestCharacter.h"

namespace
{
    struct FPerkResourceWorld
    {
        UWorld* World;
        ATestCharacter* Player;
        UCombatAbilitySystemComponent* ASC;
        UPlayerResource* Resource;

        FPerkResourceWorld()
        {
            World = UWorld::CreateWorld(EWorldType::GamePreview, false);
            GEngine->CreateNewWorldContext(EWorldType::GamePreview).SetCurrentWorld(World);
            World->InitializeActorsForPlay(FURL());
            // PostInitializeComponents initializes GAS without gameplay/UI BeginPlay dependencies.
            Player = World->SpawnActor<ATestCharacter>();
            ASC = CastChecked<UCombatAbilitySystemComponent>(Player->GetAbilitySystemComponent());
            Resource = Player->GetResource();
        }

        ~FPerkResourceWorld()
        {
            World->DestroyWorld(false);
            GEngine->DestroyWorldContext(World);
        }

        FActiveGameplayEffectHandle ApplyModifier(const FGameplayAttribute& Attribute, float Amount,
            EGameplayModOp::Type Operation = EGameplayModOp::Additive)
        {
            UGameplayEffect* Effect = NewObject<UGameplayEffect>();
            Effect->DurationPolicy = EGameplayEffectDurationType::Infinite;
            FGameplayModifierInfo& Modifier = Effect->Modifiers.AddDefaulted_GetRef();
            Modifier.Attribute = Attribute;
            Modifier.ModifierOp = Operation;
            Modifier.ModifierMagnitude = FScalableFloat(Amount);
            return ASC->ApplyGameplayEffectToSelf(Effect, 1.0f, ASC->MakeEffectContext());
        }
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPerkGoldRewardsTest, "TeamPotato.Perks.Resources.GoldRewardsAndSpending",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPerkGoldRewardsTest::RunTest(const FString& Parameters)
{
    FPerkResourceWorld Fixture;
    const FActiveGameplayEffectHandle Handle = Fixture.ApplyModifier(
        UCombatAttributeSet::GetGoldGainMultiplierAttribute(), 1.3f, EGameplayModOp::Multiplicitive);
    if (!TestTrue(TEXT("Gold perk GE is active"), Handle.IsValid())) return false;
    Fixture.Resource->AddGold(100);
    TestEqual(TEXT("One hundred earned gold receives thirty percent bonus"), Fixture.Resource->GetCurrentGold(), 130);
    Fixture.Resource->AddGold(-50);
    TestEqual(TEXT("Shop spending is never multiplied"), Fixture.Resource->GetCurrentGold(), 80);
    Fixture.ASC->RemoveActiveGameplayEffect(Handle);
    Fixture.Resource->AddGold(100);
    TestEqual(TEXT("Removing the perk restores normal rewards"), Fixture.Resource->GetCurrentGold(), 180);
    Fixture.Resource->AddGold(-1000);
    TestEqual(TEXT("Gold never becomes negative"), Fixture.Resource->GetCurrentGold(), 0);
    Fixture.Resource->AddGold(MAX_int32);
    Fixture.Resource->AddGold(1);
    TestEqual(TEXT("Gold balance does not overflow"), Fixture.Resource->GetCurrentGold(), MAX_int32);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPerkSmallGoldRewardsTest, "TeamPotato.Perks.Resources.SmallGoldRewards",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPerkSmallGoldRewardsTest::RunTest(const FString& Parameters)
{
    FPerkResourceWorld Fixture;
    Fixture.ApplyModifier(UCombatAttributeSet::GetGoldGainMultiplierAttribute(), 1.3f, EGameplayModOp::Multiplicitive);
    for (int32 Index = 0; Index < 10; ++Index) Fixture.Resource->AddGold(1);
    TestEqual(TEXT("Ten one-gold rewards receive the full thirty percent bonus"), Fixture.Resource->GetCurrentGold(), 13);
    Fixture.Resource->AddGold(-3);
    for (int32 Index = 0; Index < 10; ++Index) Fixture.Resource->AddGold(1);
    TestEqual(TEXT("Rounding stays stable across later pickups and purchases"), Fixture.Resource->GetCurrentGold(), 23);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPerkMaxEnergyTest, "TeamPotato.Perks.Resources.MaxEnergyBonus",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPerkMaxEnergyTest::RunTest(const FString& Parameters)
{
    FPerkResourceWorld Fixture;
    Fixture.Resource->SetMaxEnergy(100.0f);
    Fixture.Resource->UseEnergy(40.0f);
    const FActiveGameplayEffectHandle Handle = Fixture.ApplyModifier(UCombatAttributeSet::GetMaxEnergyBonusAttribute(), 80.0f);
    if (!TestTrue(TEXT("Energy perk GE is active"), Handle.IsValid())) return false;
    TestEqual(TEXT("The perk increases energy capacity by eighty"), Fixture.Resource->GetMaxEnergyAmount(), 180.0f);
    TestEqual(TEXT("Only the new capacity is filled, not missing energy"), Fixture.Resource->GetEnergyAmount(), 140.0f);
    TestEqual(TEXT("Reading the resource cannot refill it again"), Fixture.Resource->GetEnergyAmount(), 140.0f);
    Fixture.Resource->FillEnergy(1000.0f);
    TestEqual(TEXT("Recovery respects the increased maximum"), Fixture.Resource->GetEnergyAmount(), 180.0f);
    Fixture.Resource->SetMaxEnergy(120.0f);
    TestEqual(TEXT("Character base-stat updates retain the perk bonus"), Fixture.Resource->GetMaxEnergyAmount(), 200.0f);
    Fixture.ASC->RemoveActiveGameplayEffect(Handle);
    TestEqual(TEXT("Removal restores the latest character base maximum"), Fixture.Resource->GetMaxEnergyAmount(), 120.0f);
    TestEqual(TEXT("Removal clamps current energy to the restored cap"), Fixture.Resource->GetEnergyAmount(), 120.0f);
    Fixture.Resource->SetMaxEnergy(7.0f);
    TestTrue(TEXT("Existing small energy limits still work"), Fixture.Resource->UseEnergy(7.0f));
    TestEqual(TEXT("Exact-cost consumption leaves zero energy"), Fixture.Resource->GetEnergyAmount(), 0.0f);
    return true;
}

#endif
