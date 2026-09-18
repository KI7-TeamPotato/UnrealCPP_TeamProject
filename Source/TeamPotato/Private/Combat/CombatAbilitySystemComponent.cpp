#include "Combat/CombatAbilitySystemComponent.h"
#include "Combat/CombatAttributeSet.h"
#include "Combat/CombatEffects.h"
#include "Combat/CombatTags.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/TestCharacter.h"

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
    if (const ATestCharacter* Character = Cast<ATestCharacter>(GetOwner()))
    {
        AddLooseGameplayTag(CombatTags::Actor_Player);
        GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMoveSpeedMultiplierAttribute())
            .AddUObject(this, &ThisClass::UpdateMoveSpeed);
        RegisterGameplayTagEvent(CombatTags::State_AttackBlocked, EGameplayTagEventType::NewOrRemoved)
            .AddUObject(this, &ThisClass::HandleAttackBlockedTag);
        OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &ThisClass::HandleStatusRemoved);
        SetBaseMoveSpeed(Character->GetCharacterMovement()->MaxWalkSpeed);
    }
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

void UCombatAbilitySystemComponent::ResolveDamage(float ActualDamage, const FGameplayEffectContextHandle& Context, bool bPeriodic)
{
    if (ActualDamage <= 0.0f) return;
    if (!bPeriodic && GetHealth() > 0.0f) ApplyHitInvincibility();
    if (bPeriodic) OnCombatPeriodicDamageReceived.Broadcast(ActualDamage, Context);
    else OnCombatDamageReceived.Broadcast(ActualDamage, Context);
    if (GetHealth() <= 0.0f) MarkDead();
}

void UCombatAbilitySystemComponent::MarkDead()
{
    if (!bCombatInitialized || !IsOwnerActorAuthoritative() || bDeathNotified) return;
    bDeathNotified = true;
    RemoveAllStatuses();
    FGameplayEffectSpecHandle Spec = MakeCombatSpec(DeadEffectClass);
    if (Spec.IsValid()) ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
    SetNumericAttributeBase(UCombatAttributeSet::GetHealthAttribute(), 0.0f);
    EndDodgeInvincibility();
    CancelAllAbilities();
    OnCombatDeath.Broadcast();
}

bool UCombatAbilitySystemComponent::CanAttack() const
{
    return bCombatInitialized && !IsDead() && (!CanReceiveStatuses() || !HasMatchingGameplayTag(CombatTags::State_AttackBlocked));
}

bool UCombatAbilitySystemComponent::CanReceiveStatuses() const
{
    return IsValid(GetOwner()) && GetOwner()->IsA<ATestCharacter>();
}

void UCombatAbilitySystemComponent::SetBaseMoveSpeed(float Speed)
{
    if (!CanReceiveStatuses() || !FMath::IsFinite(Speed)) return;
    BaseMoveSpeed = FMath::Max(0.0f, Speed);
    UpdateMoveSpeed(FOnAttributeChangeData());
}

void UCombatAbilitySystemComponent::UpdateMoveSpeed(const FOnAttributeChangeData& Data)
{
    if (ATestCharacter* Character = Cast<ATestCharacter>(GetOwner()))
        Character->GetCharacterMovement()->MaxWalkSpeed = BaseMoveSpeed *
            (CombatAttributes ? FMath::Clamp(CombatAttributes->GetMoveSpeedMultiplier(), 0.0f, 10.0f) : 1.0f);
}

void UCombatAbilitySystemComponent::HandleAttackBlockedTag(FGameplayTag Tag, int32 Count)
{
    if (Count > 0) OnCombatAttackBlocked.Broadcast();
}

bool UCombatAbilitySystemComponent::HasStatus(ECombatStatus Status) const
{
    const FActiveCombatStatus* Active = ActiveStatuses.Find(Status);
    return Active && GetActiveGameplayEffect(Active->DamageHandle);
}

bool UCombatAbilitySystemComponent::ApplyStatus(UCombatStatusDefinition* Definition, const FGameplayEffectContextHandle& Context)
{
    if (!CanReceiveStatuses() || !bCombatInitialized || !IsOwnerActorAuthoritative() || IsDead() || !IsValid(Definition) ||
        !Definition->DamageEffect || !FMath::IsFinite(Definition->DamagePerTick) || Definition->DamagePerTick <= 0.0f) return false;
    const UGameplayEffect* DOT = Definition->DamageEffect.GetDefaultObject();
    // Require predictable single-stack refresh semantics. Bad editor assets must fail before side effects.
    if (DOT->DurationPolicy != EGameplayEffectDurationType::HasDuration ||
        !FMath::IsFinite(DOT->Period.GetValueAtLevel(1.0f)) || DOT->Period.GetValueAtLevel(1.0f) <= 0.0f ||
        DOT->bExecutePeriodicEffectOnApplication || DOT->StackingType != EGameplayEffectStackingType::AggregateByTarget ||
        DOT->StackLimitCount != 1 || DOT->StackDurationRefreshPolicy != EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication ||
        DOT->StackPeriodResetPolicy != EGameplayEffectStackingPeriodPolicy::NeverReset || DOT->bDenyOverflowApplication ||
        DOT->StackExpirationPolicy != EGameplayEffectStackingExpirationPolicy::ClearEntireStack ||
        DOT->OverflowEffects.Num() != 0 ||
        DOT->Modifiers.Num() != 1 || DOT->Modifiers[0].Attribute != UCombatAttributeSet::GetIncomingDamageAttribute() ||
        DOT->Modifiers[0].ModifierOp != EGameplayModOp::Additive)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid status DOT configuration: %s"), *GetNameSafe(DOT));
        return false;
    }
    if (Definition->Status == ECombatStatus::Poison)
    {
        const UGameplayEffect* Slow = Definition->PoisonSlowEffect.GetDefaultObject();
        if (!Slow || Slow->DurationPolicy != EGameplayEffectDurationType::Infinite || Slow->Period.GetValueAtLevel(1.0f) != 0.0f ||
            Slow->Modifiers.Num() != 1 || Slow->Modifiers[0].Attribute != UCombatAttributeSet::GetMoveSpeedMultiplierAttribute() ||
            Slow->Modifiers[0].ModifierOp != EGameplayModOp::Multiplicitive)
        {
            UE_LOG(LogTemp, Warning, TEXT("Poison requires a non-periodic infinite MoveSpeedMultiplier GE: %s"), *GetNameSafe(Definition));
            return false;
        }
    }
    // Different elements need different GE classes, otherwise GAS would aggregate them together.
    for (const auto& Pair : ActiveStatuses)
        if (Pair.Key != Definition->Status && Pair.Value.Definition->DamageEffect == Definition->DamageEffect) return false;

    FGameplayEffectSpecHandle Spec = MakeOutgoingSpec(Definition->DamageEffect, 1.0f, Context);
    if (!Spec.IsValid() || !FMath::IsFinite(Spec.Data->GetDuration()) || Spec.Data->GetDuration() <= 0.0f) return false;
    Spec.Data->SetSetByCallerMagnitude(CombatTags::Data_Damage, Definition->DamagePerTick);
    Spec.Data->AddDynamicAssetTag(CombatTags::Damage_Periodic);
    FGameplayTag StatusTag;
    switch (Definition->Status)
    {
    case ECombatStatus::Poison: StatusTag = CombatTags::State_Poison; break;
    case ECombatStatus::Fire: StatusTag = CombatTags::State_Fire; break;
    case ECombatStatus::Electric:
        StatusTag = CombatTags::State_Electric;
        Spec.Data->DynamicGrantedTags.AddTag(CombatTags::State_AttackBlocked);
        break;
    default: return false;
    }
    Spec.Data->DynamicGrantedTags.AddTag(StatusTag);
    // An active type retains its GE class until expiry; reapplication refreshes magnitude/source/duration.
    // This avoids restarting the periodic timer when switching weapons with the same element.
    if (const FActiveCombatStatus* Existing = ActiveStatuses.Find(Definition->Status))
    {
        if (Existing->Definition->DamageEffect != Definition->DamageEffect ||
            Existing->Definition->PoisonSlowEffect != Definition->PoisonSlowEffect) return false;
    }
    const FActiveGameplayEffectHandle Handle = ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
    if (!Handle.IsValid() || !GetActiveGameplayEffect(Handle)) return false;
    FActiveCombatStatus& Active = ActiveStatuses.FindOrAdd(Definition->Status);
    Active.Definition = Definition;
    Active.DamageHandle = Handle;
    if (Definition->Status == ECombatStatus::Poison && !Active.SlowHandle.IsValid())
    {
        FGameplayEffectSpecHandle SlowSpec = MakeOutgoingSpec(Definition->PoisonSlowEffect, 1.0f, Context);
        if (SlowSpec.IsValid()) Active.SlowHandle = ApplyGameplayEffectSpecToSelf(*SlowSpec.Data.Get());
        if (!Active.SlowHandle.IsValid())
        {
            RemoveStatus(Definition->Status);
            return false;
        }
    }
    return true;
}

void UCombatAbilitySystemComponent::RemoveStatus(ECombatStatus Status)
{
    FActiveCombatStatus Active;
    if (!IsOwnerActorAuthoritative() || !ActiveStatuses.RemoveAndCopyValue(Status, Active)) return;
    if (Active.DamageHandle.IsValid()) RemoveActiveGameplayEffect(Active.DamageHandle);
    if (Active.SlowHandle.IsValid()) RemoveActiveGameplayEffect(Active.SlowHandle);
}

void UCombatAbilitySystemComponent::RemoveAllStatuses()
{
    TArray<ECombatStatus> Types;
    ActiveStatuses.GetKeys(Types);
    for (ECombatStatus Type : Types) RemoveStatus(Type);
}

void UCombatAbilitySystemComponent::HandleStatusRemoved(const FActiveGameplayEffect& Effect)
{
    for (auto& Pair : ActiveStatuses)
    {
        if (Pair.Value.DamageHandle == Effect.Handle || Pair.Value.SlowHandle == Effect.Handle)
        {
            const ECombatStatus Type = Pair.Key;
            // The callback's GE is already being removed. Only remove its linked partner.
            if (Pair.Value.DamageHandle == Effect.Handle) Pair.Value.DamageHandle.Invalidate();
            if (Pair.Value.SlowHandle == Effect.Handle) Pair.Value.SlowHandle.Invalidate();
            RemoveStatus(Type);
            return;
        }
    }
}
