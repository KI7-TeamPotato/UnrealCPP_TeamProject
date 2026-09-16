#include "Combat/CombatAbilitySystemComponent.h"
#include "Combat/CombatAttributeSet.h"
#include "Combat/CombatEffects.h"
#include "Combat/CombatTags.h"

UCombatAbilitySystemComponent::UCombatAbilitySystemComponent()
{
    DamageEffectClass = UCombatDamageEffect::StaticClass();
    HealEffectClass = UCombatHealEffect::StaticClass();
    MaxHealthEffectClass = UCombatMaxHealthEffect::StaticClass();
    HitInvincibilityEffectClass = UCombatHitInvincibilityEffect::StaticClass();
    DodgeInvincibilityEffectClass = UCombatDodgeInvincibilityEffect::StaticClass();
    DeadEffectClass = UCombatDeadEffect::StaticClass();
}

void UCombatAbilitySystemComponent::InitializeCombat(float InitialMaxHealth, float InHitInvincibilityDuration)
{
    if (bCombatInitialized || !GetOwner()) return;
    InitAbilityActorInfo(GetOwner(), GetOwner());
    // UAttributeSet::GetOwningActor requires the Actor, not this component, as Outer.
    CombatAttributes = NewObject<UCombatAttributeSet>(GetOwner(), TEXT("CombatAttributes"));
    AddAttributeSetSubobject(CombatAttributes.Get());
    HitInvincibilityDuration = FMath::IsFinite(InHitInvincibilityDuration)
        ? FMath::Max(0.0f, InHitInvincibilityDuration) : 0.0f;
    GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetHealthAttribute())
        .AddUObject(this, &ThisClass::HandleAttributeChanged);
    GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMaxHealthAttribute())
        .AddUObject(this, &ThisClass::HandleAttributeChanged);
    bCombatInitialized = true;
    SetMaxHealthAndFill(InitialMaxHealth);
}

float UCombatAbilitySystemComponent::GetHealth() const { return CombatAttributes ? CombatAttributes->GetHealth() : 0.0f; }
float UCombatAbilitySystemComponent::GetMaxHealth() const { return CombatAttributes ? CombatAttributes->GetMaxHealth() : 1.0f; }

bool UCombatAbilitySystemComponent::IsDead() const
{
    return bDeathNotified || HasMatchingGameplayTag(CombatTags::State_Dead);
}

void UCombatAbilitySystemComponent::HandleAttributeChanged(const FOnAttributeChangeData& Data)
{
    if (!bUpdatingInitialHealth) OnCombatHealthChanged.Broadcast(GetHealth(), GetMaxHealth());
}

void UCombatAbilitySystemComponent::SetMaxHealthAndFill(float NewMaxHealth)
{
    if (!bCombatInitialized || !IsOwnerActorAuthoritative() || IsDead() || !FMath::IsFinite(NewMaxHealth)) return;
    bUpdatingInitialHealth = true;
    SetNumericAttributeBase(UCombatAttributeSet::GetMaxHealthAttribute(), FMath::Max(1.0f, NewMaxHealth));
    SetNumericAttributeBase(UCombatAttributeSet::GetHealthAttribute(), GetMaxHealth());
    bUpdatingInitialHealth = false;
    OnCombatHealthChanged.Broadcast(GetHealth(), GetMaxHealth());
}

FGameplayEffectSpecHandle UCombatAbilitySystemComponent::MakeCombatSpec(TSubclassOf<UGameplayEffect> EffectClass) const
{
    // Clearing a slot explicitly disables that effect; never silently replace an editor selection.
    return EffectClass ? MakeOutgoingSpec(EffectClass, 1.0f, MakeEffectContext()) : FGameplayEffectSpecHandle();
}

void UCombatAbilitySystemComponent::ApplyMagnitudeEffect(TSubclassOf<UGameplayEffect> EffectClass, FGameplayTag Tag, float Amount)
{
    if (!bCombatInitialized || !IsOwnerActorAuthoritative() || IsDead() || !FMath::IsFinite(Amount)) return;
    FGameplayEffectSpecHandle Spec = MakeCombatSpec(EffectClass);
    if (!Spec.IsValid()) return;
    Spec.Data->SetSetByCallerMagnitude(Tag, Amount);
    ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
}

void UCombatAbilitySystemComponent::Heal(float Amount)
{
    if (Amount > 0.0f) ApplyMagnitudeEffect(HealEffectClass, CombatTags::Data_Healing, Amount);
}

void UCombatAbilitySystemComponent::AddMaxHealth(float Amount)
{
    const float PreviousMax = GetMaxHealth();
    ApplyMagnitudeEffect(MaxHealthEffectClass, CombatTags::Data_MaxHealth, Amount);
    // Preserve the existing perk: increase max HP and heal by the actual increase.
    Heal(GetMaxHealth() - PreviousMax);
}

void UCombatAbilitySystemComponent::BeginDodgeInvincibility()
{
    if (!bCombatInitialized || !IsOwnerActorAuthoritative() || IsDead() || DodgeInvincibilityHandle.IsValid()) return;
    FGameplayEffectSpecHandle Spec = MakeCombatSpec(DodgeInvincibilityEffectClass);
    if (Spec.IsValid()) DodgeInvincibilityHandle = ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
}

void UCombatAbilitySystemComponent::EndDodgeInvincibility()
{
    if (DodgeInvincibilityHandle.IsValid())
    {
        RemoveActiveGameplayEffect(DodgeInvincibilityHandle);
        DodgeInvincibilityHandle.Invalidate();
    }
}

void UCombatAbilitySystemComponent::ApplyHitInvincibility()
{
    if (!bCombatInitialized || !IsOwnerActorAuthoritative() || IsDead()) return;
    if (bOverrideHitInvincibilityDuration && HitInvincibilityDuration <= 0.0f) return;
    FGameplayEffectSpecHandle Spec = MakeCombatSpec(HitInvincibilityEffectClass);
    if (Spec.IsValid())
    {
        if (bOverrideHitInvincibilityDuration) Spec.Data->SetDuration(HitInvincibilityDuration, true);
        ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
    }
}

void UCombatAbilitySystemComponent::ResolveDamage(float ActualDamage, const FGameplayEffectContextHandle& Context)
{
    if (ActualDamage <= 0.0f) return;
    if (GetHealth() > 0.0f) ApplyHitInvincibility();
    OnCombatDamageReceived.Broadcast(ActualDamage, Context);
    if (GetHealth() <= 0.0f) MarkDead();
}

void UCombatAbilitySystemComponent::MarkDead()
{
    if (!bCombatInitialized || !IsOwnerActorAuthoritative() || bDeathNotified) return;
    bDeathNotified = true;
    FGameplayEffectSpecHandle Spec = MakeCombatSpec(DeadEffectClass);
    if (Spec.IsValid()) ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
    SetNumericAttributeBase(UCombatAttributeSet::GetHealthAttribute(), 0.0f);
    EndDodgeInvincibility();
    CancelAllAbilities();
    OnCombatDeath.Broadcast();
}
