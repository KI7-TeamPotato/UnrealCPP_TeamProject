#include "Combat/CombatAttributeSet.h"
#include "Combat/CombatAbilitySystemComponent.h"
#include "Combat/CombatTags.h"
#include "GameplayEffectExtension.h"

UCombatAttributeSet::UCombatAttributeSet()
{
    InitHealth(100.0f);
    InitMaxHealth(100.0f);
    InitIncomingDamage(0.0f);
}

void UCombatAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
    if (!FMath::IsFinite(NewValue)) NewValue = Attribute.GetNumericValue(this);
    if (Attribute == GetMaxHealthAttribute()) NewValue = FMath::Max(1.0f, NewValue);
    if (Attribute == GetHealthAttribute()) NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    if (Attribute == GetIncomingDamageAttribute()) NewValue = FMath::Max(0.0f, NewValue);
}

void UCombatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);
    ClampAttribute(Attribute, NewValue);
}

void UCombatAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    Super::PreAttributeBaseChange(Attribute, NewValue);
    ClampAttribute(Attribute, NewValue);
}

bool UCombatAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
    if (!Super::PreGameplayEffectExecute(Data) || !FMath::IsFinite(Data.EvaluatedData.Magnitude)) return false;
    const UCombatAbilitySystemComponent* ASC = Cast<UCombatAbilitySystemComponent>(GetOwningAbilitySystemComponent());
    if (!ASC || ASC->IsDead() || GetHealth() <= 0.0f) return false;
    if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
    {
        // Also guards periodic executions and custom GEs using the damage meta attribute.
        return Data.EvaluatedData.Magnitude > 0.0f && !ASC->HasMatchingGameplayTag(CombatTags::State_Invincible);
    }
    return true;
}

void UCombatAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
    UCombatAbilitySystemComponent* ASC = Cast<UCombatAbilitySystemComponent>(GetOwningAbilitySystemComponent());
    if (!ASC) return;

    if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
    {
        const float Damage = GetIncomingDamage();
        SetIncomingDamage(0.0f);
        const float PreviousHealth = GetHealth();
        SetHealth(FMath::Clamp(PreviousHealth - Damage, 0.0f, GetMaxHealth()));
        ASC->ResolveDamage(PreviousHealth - GetHealth(), Data.EffectSpec.GetContext());
    }
    else if (Data.EvaluatedData.Attribute == GetHealthAttribute() || Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
        if (GetHealth() <= 0.0f) ASC->MarkDead();
    }
}
