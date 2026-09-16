#include "Combat/CombatEffects.h"
#include "Combat/CombatAttributeSet.h"
#include "Combat/CombatTags.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "GameplayEffectComponents/TargetTagRequirementsGameplayEffectComponent.h"

namespace
{
    void AddSetByCallerModifier(UGameplayEffect& Effect, const FGameplayAttribute& Attribute, FGameplayTag Tag)
    {
        FSetByCallerFloat Magnitude;
        Magnitude.DataTag = Tag;
        FGameplayModifierInfo& Modifier = Effect.Modifiers.AddDefaulted_GetRef();
        Modifier.Attribute = Attribute;
        Modifier.ModifierOp = EGameplayModOp::Additive;
        Modifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(Magnitude);
    }

}

UCombatGameplayEffect::UCombatGameplayEffect()
{
    // GE components created inside constructors must be named default subobjects.
    GrantedTags = CreateDefaultSubobject<UTargetTagsGameplayEffectComponent>(TEXT("GrantedTags"));
    TargetRequirements = CreateDefaultSubobject<UTargetTagRequirementsGameplayEffectComponent>(TEXT("TargetRequirements"));
    GEComponents.Add(GrantedTags);
    GEComponents.Add(TargetRequirements);
}

void UCombatGameplayEffect::GrantCombatTag(FGameplayTag Tag)
{
    FInheritedTagContainer Tags;
    Tags.AddTag(Tag);
    GrantedTags->SetAndApplyTargetTagChanges(Tags);
}

void UCombatGameplayEffect::BlockDeadTargets()
{
    TargetRequirements->ApplicationTagRequirements.IgnoreTags.AddTag(CombatTags::State_Dead);
}

UCombatDamageEffect::UCombatDamageEffect()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;
    AddSetByCallerModifier(*this, UCombatAttributeSet::GetIncomingDamageAttribute(), CombatTags::Data_Damage);
    BlockDeadTargets();
    TargetRequirements->ApplicationTagRequirements.IgnoreTags.AddTag(CombatTags::State_Invincible);
}

UCombatHealEffect::UCombatHealEffect()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;
    AddSetByCallerModifier(*this, UCombatAttributeSet::GetHealthAttribute(), CombatTags::Data_Healing);
    BlockDeadTargets();
}

UCombatMaxHealthEffect::UCombatMaxHealthEffect()
{
    DurationPolicy = EGameplayEffectDurationType::Instant;
    AddSetByCallerModifier(*this, UCombatAttributeSet::GetMaxHealthAttribute(), CombatTags::Data_MaxHealth);
    BlockDeadTargets();
}

UCombatHitInvincibilityEffect::UCombatHitInvincibilityEffect()
{
    DurationPolicy = EGameplayEffectDurationType::HasDuration;
    DurationMagnitude = FScalableFloat(1.0f);
    GrantCombatTag(CombatTags::State_Invincible);
    BlockDeadTargets();
}

UCombatDodgeInvincibilityEffect::UCombatDodgeInvincibilityEffect()
{
    DurationPolicy = EGameplayEffectDurationType::Infinite;
    GrantCombatTag(CombatTags::State_Invincible);
    BlockDeadTargets();
}

UCombatDeadEffect::UCombatDeadEffect()
{
    DurationPolicy = EGameplayEffectDurationType::Infinite;
    GrantCombatTag(CombatTags::State_Dead);
}
