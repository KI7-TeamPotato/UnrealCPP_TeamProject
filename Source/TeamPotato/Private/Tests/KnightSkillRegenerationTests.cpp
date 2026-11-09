#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Combat/CombatAbilitySystemComponent.h"
#include "Combat/CombatAttributeSet.h"
#include "Combat/CombatFunctionLibrary.h"
#include "Combat/CombatTags.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffect.h"
#include "Player/TestCharacter.h"

namespace
{
    struct FKnightRegenerationWorld
    {
        UWorld* World;
        ATestCharacter* Player = nullptr;
        UCombatAbilitySystemComponent* ASC = nullptr;
        TSubclassOf<UGameplayEffect> BuffClass;
        TSubclassOf<UGameplayEffect> MovementClass;
        float BuffDuration = 0.0f;
        float HealPeriod = 0.0f;
        const uint64 InitialFrame = GFrameCounter;

        FKnightRegenerationWorld()
        {
            World = UWorld::CreateWorld(EWorldType::GamePreview, false);
            GEngine->CreateNewWorldContext(EWorldType::GamePreview).SetCurrentWorld(World);
            World->InitializeActorsForPlay(FURL());
        }

        ~FKnightRegenerationWorld()
        {
            World->DestroyWorld(false);
            GEngine->DestroyWorldContext(World);
            GFrameCounter = InitialFrame;
        }

        bool SpawnKnight(float InitialHealth = 20.0f)
        {
            UClass* PlayerClass = LoadClass<ATestCharacter>(nullptr,
                TEXT("/Game/BluePrints/Test/Player/BP_PlayerKnight.BP_PlayerKnight_C"));
            BuffClass = LoadClass<UGameplayEffect>(nullptr,
                TEXT("/Game/Effect/GE_KnightSkillBuff.GE_KnightSkillBuff_C"));
            MovementClass = LoadClass<UGameplayEffect>(nullptr,
                TEXT("/Game/Effect/GE_KnightSkillMovement.GE_KnightSkillMovement_C"));
            if (!PlayerClass || !BuffClass || !MovementClass) return false;
            Player = World->SpawnActor<ATestCharacter>(PlayerClass, FVector(0, 0, 100), FRotator::ZeroRotator);
            if (!Player) return false;
            ASC = Cast<UCombatAbilitySystemComponent>(Player->GetAbilitySystemComponent());
            if (!ASC || !ASC->IsCombatInitialized()) return false;
            const FGameplayEffectSpecHandle BuffSpec = ASC->MakeOutgoingSpec(BuffClass, 1.0f, ASC->MakeEffectContext());
            if (!BuffSpec.IsValid()) return false;
            BuffDuration = BuffSpec.Data->GetDuration();
            HealPeriod = BuffSpec.Data->GetPeriod();
            if (!FMath::IsFinite(BuffDuration) || BuffDuration <= 0.0f || !FMath::IsFinite(HealPeriod) || HealPeriod <= 0.0f) return false;
            // PostInitializeComponents grants the real character skill without unrelated UI BeginPlay.
            ASC->SetMaxHealthAndFill(100.0f);
            SetHealth(InitialHealth);
            return true;
        }

        void SetHealth(float Health)
        {
            ASC->SetNumericAttributeBase(UCombatAttributeSet::GetHealthAttribute(), Health);
        }

        float SpeedMultiplier() const
        {
            return ASC->GetNumericAttribute(UCombatAttributeSet::GetMoveSpeedMultiplierAttribute());
        }

        TArray<FActiveGameplayEffectHandle> ActiveEffects(TSubclassOf<UGameplayEffect> Class) const
        {
            FGameplayEffectQuery Query;
            Query.EffectDefinition = Class;
            return ASC->GetActiveEffects(Query);
        }

        FActiveGameplayEffectHandle Apply(TSubclassOf<UGameplayEffect> Class)
        {
            const FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(Class, 1.0f, ASC->MakeEffectContext());
            return Spec.IsValid() ? ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get()) : FActiveGameplayEffectHandle();
        }

        void Advance(float Seconds)
        {
            const int32 Steps = FMath::RoundToInt(Seconds / 0.025f);
            for (int32 Step = 0; Step < Steps; ++Step)
            {
                ++GFrameCounter;
                World->Tick(LEVELTICK_All, 0.025f);
            }
        }
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FKnightRegenerationLifecycleTest, "TeamPotato.Combat.KnightSkill.RegenerationLifecycle",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FKnightRegenerationLifecycleTest::RunTest(const FString& Parameters)
{
    FKnightRegenerationWorld Fixture;
    if (!TestTrue(TEXT("Actual Knight and buff assets load"), Fixture.SpawnKnight())) return false;
    if (!TestEqual(TEXT("Authored regeneration still ticks once per second"), Fixture.HealPeriod, 1.0f)) return false;
    const float BaseSpeed = Fixture.Player->GetCharacterMovement()->MaxWalkSpeed;
    if (!TestTrue(TEXT("Knight input activates its automatically granted skill"), Fixture.Player->TryUseCharacterSkill())) return false;
    TestEqual(TEXT("Skill applies one duration buff"), Fixture.ActiveEffects(Fixture.BuffClass).Num(), 1);
    TestEqual(TEXT("Buff applies one movement effect"), Fixture.ActiveEffects(Fixture.MovementClass).Num(), 1);
    TestEqual(TEXT("Casting does not heal immediately"), Fixture.ASC->GetHealth(), 20.0f);
    TestTrue(TEXT("Damage reduction is preserved"), Fixture.ASC->HasMatchingGameplayTag(CombatTags::State_DamageReduction));
    TestTrue(TEXT("Knight lifecycle tag is granted"), Fixture.ASC->HasMatchingGameplayTag(CombatTags::State_KnightBuff));
    TestTrue(TEXT("Existing persistent cue is active"), Fixture.ASC->HasMatchingGameplayTag(CombatTags::GameplayCue_KnightBuff));
    TestTrue(TEXT("Movement bonus applies immediately"), FMath::IsNearlyEqual(Fixture.SpeedMultiplier(), 1.3f));
    Fixture.Advance(0.9f);
    TestEqual(TEXT("No heal before the first second"), Fixture.ASC->GetHealth(), 20.0f);
    Fixture.Advance(0.2f);

    float SampleTime = 1.1f;
    for (int32 Tick = 1; SampleTime < Fixture.BuffDuration; ++Tick)
    {
        TestEqual(FString::Printf(TEXT("Second %d heals ten health, up to maximum"), Tick),
            Fixture.ASC->GetHealth(), FMath::Min(100.0f, 20.0f + Tick * 10.0f));
        TestTrue(TEXT("Periodic healing does not compound movement speed"), FMath::IsNearlyEqual(Fixture.SpeedMultiplier(), 1.3f));
        TestTrue(TEXT("Character movement receives the active bonus"),
            FMath::IsNearlyEqual(Fixture.Player->GetCharacterMovement()->MaxWalkSpeed, BaseSpeed * 1.3f, 0.01f));
        TestEqual(TEXT("Periodic execution does not duplicate the persistent cue"), Fixture.ASC->GetTagCount(CombatTags::GameplayCue_KnightBuff), 1);
        Fixture.Advance(1.0f);
        SampleTime += 1.0f;
    }

    if (SampleTime < Fixture.BuffDuration + 0.1f) Fixture.Advance(Fixture.BuffDuration + 0.1f - SampleTime);
    const int32 TotalHealTicks = FMath::FloorToInt((Fixture.BuffDuration + KINDA_SMALL_NUMBER) / Fixture.HealPeriod);
    const float ExpectedFinalHealth = FMath::Min(100.0f, 20.0f + TotalHealTicks * 10.0f);
    TestEqual(TEXT("Authored-duration expiry includes every complete healing period"), Fixture.ASC->GetHealth(), ExpectedFinalHealth);
    TestEqual(TEXT("Duration buff expires"), Fixture.ActiveEffects(Fixture.BuffClass).Num(), 0);
    TestEqual(TEXT("Linked movement effect is removed"), Fixture.ActiveEffects(Fixture.MovementClass).Num(), 0);
    TestFalse(TEXT("Damage reduction ends"), Fixture.ASC->HasMatchingGameplayTag(CombatTags::State_DamageReduction));
    TestFalse(TEXT("Knight lifecycle tag ends"), Fixture.ASC->HasMatchingGameplayTag(CombatTags::State_KnightBuff));
    TestFalse(TEXT("Persistent cue ends with the buff"), Fixture.ASC->HasMatchingGameplayTag(CombatTags::GameplayCue_KnightBuff));
    TestTrue(TEXT("Movement multiplier returns to its base"), FMath::IsNearlyEqual(Fixture.SpeedMultiplier(), 1.0f));
    TestTrue(TEXT("Walk speed returns to its base"), FMath::IsNearlyEqual(Fixture.Player->GetCharacterMovement()->MaxWalkSpeed, BaseSpeed));
    Fixture.Advance(1.2f);
    TestEqual(TEXT("Healing stops after expiry"), Fixture.ASC->GetHealth(), ExpectedFinalHealth);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FKnightRegenerationRemovalTest, "TeamPotato.Combat.KnightSkill.EarlyRemovalAndDeath",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FKnightRegenerationRemovalTest::RunTest(const FString& Parameters)
{
    {
        FKnightRegenerationWorld Fixture;
        if (!TestTrue(TEXT("Knight fixture loads"), Fixture.SpawnKnight()) ||
            !TestTrue(TEXT("Skill activates before early removal"), Fixture.Player->TryUseCharacterSkill())) return false;
        Fixture.Advance(2.1f);
        TestEqual(TEXT("Two ticks occurred before removal"), Fixture.ASC->GetHealth(), 40.0f);
        const TArray<FActiveGameplayEffectHandle> Buffs = Fixture.ActiveEffects(Fixture.BuffClass);
        if (!TestEqual(TEXT("One buff can be removed"), Buffs.Num(), 1)) return false;
        TestTrue(TEXT("Explicit removal succeeds"), Fixture.ASC->RemoveActiveGameplayEffect(Buffs[0]));
        TestEqual(TEXT("Early removal immediately cleans up movement"), Fixture.ActiveEffects(Fixture.MovementClass).Num(), 0);
        TestTrue(TEXT("Early removal restores movement multiplier"), FMath::IsNearlyEqual(Fixture.SpeedMultiplier(), 1.0f));
        TestFalse(TEXT("Early removal ends the cue"), Fixture.ASC->HasMatchingGameplayTag(CombatTags::GameplayCue_KnightBuff));
        Fixture.Advance(Fixture.BuffDuration + 1.0f);
        TestEqual(TEXT("Early removal grants no remaining or final heal"), Fixture.ASC->GetHealth(), 40.0f);
    }

    {
        FKnightRegenerationWorld Fixture;
        if (!TestTrue(TEXT("Second Knight fixture loads"), Fixture.SpawnKnight()) ||
            !TestTrue(TEXT("Skill activates before death"), Fixture.Player->TryUseCharacterSkill())) return false;
        Fixture.Advance(1.1f);
        Fixture.ASC->MarkDead();
        TestTrue(TEXT("Character is dead"), Fixture.ASC->IsDead());
        TestEqual(TEXT("Death removes the healing buff"), Fixture.ActiveEffects(Fixture.BuffClass).Num(), 0);
        TestEqual(TEXT("Death removes its linked movement effect"), Fixture.ActiveEffects(Fixture.MovementClass).Num(), 0);
        TestFalse(TEXT("Death ends the persistent cue"), Fixture.ASC->HasMatchingGameplayTag(CombatTags::GameplayCue_KnightBuff));
        Fixture.Advance(Fixture.BuffDuration + 1.0f);
        TestEqual(TEXT("Regeneration cannot revive a dead character"), Fixture.ASC->GetHealth(), 0.0f);
        TestFalse(TEXT("Dead Knight cannot reactivate the skill"), Fixture.Player->TryUseCharacterSkill());
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FKnightRegenerationRefreshTest, "TeamPotato.Combat.KnightSkill.ClampingAndRefresh",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FKnightRegenerationRefreshTest::RunTest(const FString& Parameters)
{
    FKnightRegenerationWorld Fixture;
    if (!TestTrue(TEXT("Knight fixture loads"), Fixture.SpawnKnight(95.0f))) return false;
    if (!TestEqual(TEXT("Refreshed regeneration still ticks once per second"), Fixture.HealPeriod, 1.0f)) return false;
    TestFalse(TEXT("Movement child cannot apply without the Knight buff"), Fixture.Apply(Fixture.MovementClass).IsValid());
    TestTrue(TEXT("Rejected movement effect leaves speed unchanged"), FMath::IsNearlyEqual(Fixture.SpeedMultiplier(), 1.0f));
    if (!TestTrue(TEXT("Knight skill activates"), Fixture.Player->TryUseCharacterSkill())) return false;
    AActor* DamageCauser = Fixture.World->SpawnActor<AActor>();
    TestEqual(TEXT("Existing buff still reduces incoming damage by thirty percent"),
        UCombatFunctionLibrary::ApplyCombatDamage(Fixture.Player, 10.0f, DamageCauser), 7.0f);
    Fixture.SetHealth(95.0f);
    Fixture.Advance(0.4f);
    TestTrue(TEXT("Direct reapplication refreshes the same buff"), Fixture.Apply(Fixture.BuffClass).IsValid());
    TestEqual(TEXT("Reapplication does not create a second healing effect"), Fixture.ActiveEffects(Fixture.BuffClass).Num(), 1);
    TestEqual(TEXT("Reapplication does not create a second movement effect"), Fixture.ActiveEffects(Fixture.MovementClass).Num(), 1);
    Fixture.Advance(0.7f);
    TestEqual(TEXT("First heal keeps its original schedule and clamps to maximum health"), Fixture.ASC->GetHealth(), 100.0f);
    Fixture.SetHealth(20.0f);
    Fixture.Advance(1.0f);
    TestEqual(TEXT("Refreshed buff still heals only ten per period"), Fixture.ASC->GetHealth(), 30.0f);
    TestTrue(TEXT("Refreshed movement bonus remains thirty percent"), FMath::IsNearlyEqual(Fixture.SpeedMultiplier(), 1.3f));
    // Reapplication at t=0.4 must retain the buff just past its original authored expiry.
    const float RefreshedSampleTime = Fixture.BuffDuration + 0.2f;
    Fixture.Advance(RefreshedSampleTime - 2.1f);
    TestTrue(TEXT("Reapplication refreshes duration past the original expiry"), Fixture.ASC->HasMatchingGameplayTag(CombatTags::State_KnightBuff));
    const int32 TicksAtSample = FMath::FloorToInt((RefreshedSampleTime + KINDA_SMALL_NUMBER) / Fixture.HealPeriod);
    TestEqual(TEXT("Refreshed buff keeps the original one-second tick schedule"), Fixture.ASC->GetHealth(),
        FMath::Min(100.0f, 20.0f + (TicksAtSample - 1) * 10.0f));
    Fixture.Advance(0.4f);
    TestEqual(TEXT("Refreshed duration also cleans up movement"), Fixture.ActiveEffects(Fixture.MovementClass).Num(), 0);
    const int32 TicksAtExpiry = FMath::FloorToInt((Fixture.BuffDuration + 0.4f + KINDA_SMALL_NUMBER) / Fixture.HealPeriod);
    TestEqual(TEXT("Refreshed expiry grants only complete original-schedule periods"), Fixture.ASC->GetHealth(),
        FMath::Min(100.0f, 20.0f + (TicksAtExpiry - 1) * 10.0f));
    return true;
}

#endif
