#include "Player/Perk/GameplayEffectPerkEffect.h"
#include "AbilitySystemGlobals.h"
#include "Combat/CombatAbilitySystemComponent.h"
#include "GameplayEffect.h"

bool UGameplayEffectPerkEffect::OnEquip(AActor* OwnerCharacter)
{
    if (!IsValid(OwnerCharacter) || EffectHandle.IsValid()) return false;
    auto* ASC = Cast<UCombatAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerCharacter, true));
    const UGameplayEffect* Effect = EffectClass.GetDefaultObject();
    if (!ASC || !ASC->IsCombatInitialized() || !OwnerCharacter->HasAuthority() || ASC->IsDead() ||
        !Effect || Effect->DurationPolicy != EGameplayEffectDurationType::Infinite || Effect->Period.GetValueAtLevel(1.0f) != 0.0f) return false;

    auto Context = ASC->MakeEffectContext();
    Context.AddSourceObject(this);
    auto Spec = ASC->MakeOutgoingSpec(EffectClass, 1.0f, Context);
    if (!Spec.IsValid()) return false;
    const float PreviousMaxHealth = ASC->GetMaxHealth();
    EffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
    if (!EffectHandle.IsValid() || !ASC->GetActiveGameplayEffect(EffectHandle))
    {
        EffectHandle.Invalidate();
        return false;
    }
    AppliedTo = ASC;
    // Match existing max-health perks: fill the added capacity, without healing existing damage.
    ASC->Heal(FMath::Max(0.0f, ASC->GetMaxHealth() - PreviousMaxHealth));
    return true;
}

void UGameplayEffectPerkEffect::OnUnequip(AActor* OwnerCharacter)
{
    if (AppliedTo.IsValid() && EffectHandle.IsValid())
        AppliedTo->RemoveActiveGameplayEffect(EffectHandle);
    EffectHandle.Invalidate();
    AppliedTo.Reset();
}
