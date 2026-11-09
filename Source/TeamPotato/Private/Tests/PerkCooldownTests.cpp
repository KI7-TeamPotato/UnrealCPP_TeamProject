#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Abilities/GameplayAbility.h"
#include "Combat/CombatAbilitySystemComponent.h"
#include "Combat/CombatAttributeSet.h"
#include "Combat/CombatEffects.h"
#include "Combat/CombatTags.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameplayEffect.h"
#include "TimerManager.h"

namespace
{
    struct FPerkCooldownWorld
    {
        UWorld* World;
        UCombatAbilitySystemComponent* ASC;
        uint64 InitialFrame = GFrameCounter;

        FPerkCooldownWorld()
        {
            World = UWorld::CreateWorld(EWorldType::GamePreview, false);
            GEngine->CreateNewWorldContext(EWorldType::GamePreview).SetCurrentWorld(World);
            World->InitializeActorsForPlay(FURL());
            AActor* Actor = World->SpawnActor<AActor>();
            ASC = NewObject<UCombatAbilitySystemComponent>(Actor);
            Actor->AddInstanceComponent(ASC);
            ASC->RegisterComponent();
            ASC->InitializeCombat(100.0f, 0.0f);
            World->GetTimerManager().Tick(0.0f);
        }

        ~FPerkCooldownWorld()
        {
            World->DestroyWorld(false);
            GEngine->DestroyWorldContext(World);
            GFrameCounter = InitialFrame;
        }

        void SetMultiplier(float Multiplier)
        {
            ASC->SetNumericAttributeBase(UCombatAttributeSet::GetCooldownDurationMultiplierAttribute(), Multiplier);
        }

        void Advance(float Seconds)
        {
            // GAS rechecks the world's clock when its duration timer fires.
            // Ticking only TimerManager leaves World::TimeSeconds at zero and reschedules expiry.
            for (float Remaining = Seconds; Remaining > KINDA_SMALL_NUMBER;)
            {
                const float Step = FMath::Min(0.025f, Remaining);
                ++GFrameCounter;
                World->Tick(LEVELTICK_All, Step);
                Remaining -= Step;
            }
        }

        TArray<FActiveGameplayEffectHandle> Effects(TSubclassOf<UGameplayEffect> Class) const
        {
            FGameplayEffectQuery Query;
            Query.EffectDefinition = Class;
            return ASC->GetActiveEffects(Query);
        }
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPerkAllSkillCooldownsTest, "TeamPotato.Perks.Cooldown.AllCharacterSkills",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPerkAllSkillCooldownsTest::RunTest(const FString& Parameters)
{
    FPerkCooldownWorld Fixture;
    Fixture.SetMultiplier(0.5f);
    const TCHAR* AbilityPaths[] = {
        TEXT("/Game/GameplayAbility/GA_KnightSkill.GA_KnightSkill_C"),
        TEXT("/Game/GameplayAbility/GA_SommonTurret.GA_SommonTurret_C"),
        TEXT("/Game/GameplayAbility/GA_RangeSkill.GA_RangeSkill_C")
    };

    for (const TCHAR* Path : AbilityPaths)
    {
        UClass* AbilityClass = LoadClass<UGameplayAbility>(nullptr, Path);
        if (!TestNotNull(FString::Printf(TEXT("Configured ability loads: %s"), Path), AbilityClass)) return false;
        const auto* Ability = AbilityClass->GetDefaultObject<UGameplayAbility>();
        const UGameplayEffect* Cooldown = Ability->GetCooldownGameplayEffect();
        if (!TestNotNull(TEXT("Character ability has its configured cooldown"), Cooldown)) return false;
        const FGameplayEffectSpecHandle Original = Fixture.ASC->MakeOutgoingSpec(Cooldown->GetClass(), 1.0f, Fixture.ASC->MakeEffectContext());
        if (!TestTrue(TEXT("Configured cooldown spec can be created"), Original.IsValid())) return false;
        const float OriginalDuration = Original.Data->GetDuration();
        TestTrue(TEXT("Cooldown has a finite positive duration"), FMath::IsFinite(OriginalDuration) && OriginalDuration > 0.0f);
        const FGameplayAbilitySpecHandle Handle = Fixture.ASC->GiveAbility(FGameplayAbilitySpec(AbilityClass));
        // Exercise the same GAS ApplyCooldown route used by native and Blueprint CommitAbility.
        Ability->ApplyCooldown(Handle, Fixture.ASC->AbilityActorInfo.Get(), FGameplayAbilityActivationInfo());
        const TArray<FActiveGameplayEffectHandle> Active = Fixture.Effects(Cooldown->GetClass());
        if (!TestEqual(TEXT("Ability applies one cooldown"), Active.Num(), 1)) return false;
        const FActiveGameplayEffect* Applied = Fixture.ASC->GetActiveGameplayEffect(Active[0]);
        if (!TestNotNull(TEXT("Applied cooldown is active"), Applied)) return false;
        TestTrue(TEXT("Real ability cooldown is reduced by exactly fifty percent"),
            FMath::IsNearlyEqual(Applied->Spec.GetDuration(), OriginalDuration * 0.5f));
        Fixture.ASC->RemoveActiveGameplayEffect(Active[0]);
        Fixture.ASC->ClearAbility(Handle);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPerkCooldownLifecycleTest, "TeamPotato.Perks.Cooldown.LifecycleAndSpecReuse",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPerkCooldownLifecycleTest::RunTest(const FString& Parameters)
{
    FPerkCooldownWorld Fixture;
    UClass* CooldownClass = LoadClass<UGameplayEffect>(nullptr,
        TEXT("/Game/Effect/GE_RangeSkillCoolDown.GE_RangeSkillCoolDown_C"));
    if (!TestNotNull(TEXT("Range cooldown asset loads"), CooldownClass)) return false;
    const FGameplayEffectSpecHandle Spec = Fixture.ASC->MakeOutgoingSpec(CooldownClass, 1.0f, Fixture.ASC->MakeEffectContext());
    if (!TestTrue(TEXT("Cooldown spec is valid"), Spec.IsValid())) return false;
    const float OriginalDuration = Spec.Data->GetDuration();
    const FActiveGameplayEffectHandle Existing = Fixture.ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
    Fixture.SetMultiplier(0.5f);
    const FActiveGameplayEffect* ExistingEffect = Fixture.ASC->GetActiveGameplayEffect(Existing);
    if (!TestNotNull(TEXT("Existing cooldown remains active"), ExistingEffect)) return false;
    TestTrue(TEXT("Acquiring the perk does not rewrite an already running cooldown"),
        FMath::IsNearlyEqual(ExistingEffect->Spec.GetDuration(), OriginalDuration));
    Fixture.ASC->RemoveActiveGameplayEffect(Existing);

    for (int32 Use = 0; Use < 2; ++Use)
    {
        const FActiveGameplayEffectHandle Reduced = Fixture.ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
        const FActiveGameplayEffect* Applied = Fixture.ASC->GetActiveGameplayEffect(Reduced);
        if (!TestNotNull(TEXT("Reusable cooldown spec applies"), Applied)) return false;
        TestTrue(TEXT("Reusing a spec halves it once per application"),
            FMath::IsNearlyEqual(Applied->Spec.GetDuration(), OriginalDuration * 0.5f));
        TestTrue(TEXT("The caller's original spec duration stays unchanged"),
            FMath::IsNearlyEqual(Spec.Data->GetDuration(), OriginalDuration));
        Fixture.ASC->RemoveActiveGameplayEffect(Reduced);
    }

    const FActiveGameplayEffectHandle Reduced = Fixture.ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
    Fixture.SetMultiplier(1.0f);
    const FActiveGameplayEffect* ReducedEffect = Fixture.ASC->GetActiveGameplayEffect(Reduced);
    if (!TestNotNull(TEXT("Removing the perk preserves the running cooldown"), ReducedEffect)) return false;
    TestTrue(TEXT("Running reduced cooldown retains the committed duration"),
        FMath::IsNearlyEqual(ReducedEffect->Spec.GetDuration(), OriginalDuration * 0.5f));
    Fixture.Advance(OriginalDuration * 0.5f - 0.05f);
    TestTrue(TEXT("Cooldown still blocks just before the shortened duration"),
        Fixture.ASC->HasMatchingGameplayTag(CombatTags::Cooldown_RangeSkillCoolDown));
    Fixture.Advance(0.1f);
    TestFalse(TEXT("Cooldown tag expires at the shortened duration"),
        Fixture.ASC->HasMatchingGameplayTag(CombatTags::Cooldown_RangeSkillCoolDown));

    const FActiveGameplayEffectHandle Restored = Fixture.ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
    const FActiveGameplayEffect* RestoredEffect = Fixture.ASC->GetActiveGameplayEffect(Restored);
    if (!TestNotNull(TEXT("Cooldown applies after perk removal"), RestoredEffect)) return false;
    TestTrue(TEXT("Subsequent cooldowns return to their normal duration"),
        FMath::IsNearlyEqual(RestoredEffect->Spec.GetDuration(), OriginalDuration));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPerkCooldownBuffIsolationTest, "TeamPotato.Perks.Cooldown.PreservesOtherDurations",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPerkCooldownBuffIsolationTest::RunTest(const FString& Parameters)
{
    FPerkCooldownWorld Fixture;
    Fixture.SetMultiplier(0.5f);
    // These player-only effects use the same ASC without needing UI or a character BeginPlay.
    Fixture.ASC->AddLooseGameplayTag(CombatTags::Actor_Player);
    UClass* KnightBuff = LoadClass<UGameplayEffect>(nullptr,
        TEXT("/Game/Effect/GE_KnightSkillBuff.GE_KnightSkillBuff_C"));
    if (!TestNotNull(TEXT("Knight's actual buff loads"), KnightBuff)) return false;
    for (TSubclassOf<UGameplayEffect> Class : {TSubclassOf<UGameplayEffect>(KnightBuff),
        TSubclassOf<UGameplayEffect>(UCombatHitInvincibilityEffect::StaticClass())})
    {
        const FGameplayEffectSpecHandle Spec = Fixture.ASC->MakeOutgoingSpec(Class, 1.0f, Fixture.ASC->MakeEffectContext());
        if (!TestTrue(TEXT("Non-cooldown duration spec is valid"), Spec.IsValid())) return false;
        const float OriginalDuration = Spec.Data->GetDuration();
        const FActiveGameplayEffectHandle Handle = Fixture.ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
        const FActiveGameplayEffect* Active = Fixture.ASC->GetActiveGameplayEffect(Handle);
        if (!TestNotNull(TEXT("Non-cooldown duration effect applies"), Active)) return false;
        TestTrue(TEXT("Buff and invincibility durations are not reduced"),
            FMath::IsNearlyEqual(Active->Spec.GetDuration(), OriginalDuration));
        Fixture.ASC->RemoveActiveGameplayEffect(Handle);
    }
    return true;
}

#endif
