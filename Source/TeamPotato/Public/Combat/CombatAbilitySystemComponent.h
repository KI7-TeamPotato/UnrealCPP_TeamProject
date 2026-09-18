#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Combat/CombatStatusDefinition.h"
#include "CombatAbilitySystemComponent.generated.h"

class UCombatAttributeSet;

DECLARE_MULTICAST_DELEGATE_TwoParams(FCombatHealthChanged, float, float);
DECLARE_MULTICAST_DELEGATE_TwoParams(FCombatDamageReceived, float, const FGameplayEffectContextHandle&);
DECLARE_MULTICAST_DELEGATE(FCombatDeath);
DECLARE_MULTICAST_DELEGATE(FCombatAttackBlocked);

USTRUCT()
struct FActiveCombatStatus
{
    GENERATED_BODY()
    UPROPERTY() TObjectPtr<UCombatStatusDefinition> Definition;
    FActiveGameplayEffectHandle DamageHandle;
    FActiveGameplayEffectHandle SlowHandle;
};

UCLASS(ClassGroup = (Combat), meta = (BlueprintSpawnableComponent))
class TEAMPOTATO_API UCombatAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:
    UCombatAbilitySystemComponent();
    void InitializeCombat(float InitialMaxHealth, float InHitInvincibilityDuration);
    bool IsCombatInitialized() const { return bCombatInitialized; }

    UFUNCTION(BlueprintPure, Category = "Combat")
    float GetHealth() const;
    UFUNCTION(BlueprintPure, Category = "Combat")
    float GetMaxHealth() const;
    UFUNCTION(BlueprintPure, Category = "Combat")
    bool IsDead() const;

    // Assign editor-authored GameplayEffect Blueprints on the inherited component.
    // Damage is selected on the attacker; hazards without a combat ASC use the target's class.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Effects")
    TSubclassOf<UGameplayEffect> DamageEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Effects")
    TSubclassOf<UGameplayEffect> HealEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Effects")
    TSubclassOf<UGameplayEffect> MaxHealthEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Effects")
    TSubclassOf<UGameplayEffect> HitInvincibilityEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Effects")
    TSubclassOf<UGameplayEffect> DodgeInvincibilityEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Effects")
    TSubclassOf<UGameplayEffect> DeadEffectClass;

    // Disable to use Duration Magnitude from the selected hit-invincibility GE.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Effects")
    bool bOverrideHitInvincibilityDuration = true;

    // Initial character selection retains the old full-health reset semantics; never revives.
    void SetMaxHealthAndFill(float NewMaxHealth);
    void AddMaxHealth(float Amount);
    void Heal(float Amount);
    void BeginDodgeInvincibility();
    void EndDodgeInvincibility();
    void ApplyHitInvincibility();
    void MarkDead();
    void ResolveDamage(float ActualDamage, const FGameplayEffectContextHandle& Context, bool bPeriodic = false);

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool CanAttack() const;

    // Only TestCharacter and its player Blueprint subclasses receive elemental statuses.
    UFUNCTION(BlueprintPure, Category = "Combat|Status")
    bool CanReceiveStatuses() const;

    UFUNCTION(BlueprintCallable, Category = "Combat|Movement")
    void SetBaseMoveSpeed(float Speed);
    UFUNCTION(BlueprintPure, Category = "Combat|Movement")
    float GetBaseMoveSpeed() const { return BaseMoveSpeed; }

    bool ApplyStatus(UCombatStatusDefinition* Definition, const FGameplayEffectContextHandle& Context);
    UFUNCTION(BlueprintCallable, Category = "Combat|Status")
    void RemoveStatus(ECombatStatus Status);
    UFUNCTION(BlueprintCallable, Category = "Combat|Status")
    void RemoveAllStatuses();
    UFUNCTION(BlueprintPure, Category = "Combat|Status")
    bool HasStatus(ECombatStatus Status) const;

    FCombatHealthChanged OnCombatHealthChanged;
    FCombatDamageReceived OnCombatDamageReceived;
    FCombatDeath OnCombatDeath;
    FCombatAttackBlocked OnCombatAttackBlocked;
    FCombatDamageReceived OnCombatPeriodicDamageReceived;

private:
    void UpdateMoveSpeed(const FOnAttributeChangeData& Data);
    void HandleAttackBlockedTag(FGameplayTag Tag, int32 Count);
    void HandleStatusRemoved(const FActiveGameplayEffect& Effect);
    UPROPERTY() TMap<ECombatStatus, FActiveCombatStatus> ActiveStatuses;
    float BaseMoveSpeed = 0.0f;
    FGameplayEffectSpecHandle MakeCombatSpec(TSubclassOf<UGameplayEffect> EffectClass) const;
    void HandleAttributeChanged(const FOnAttributeChangeData& Data);
    void ApplyMagnitudeEffect(TSubclassOf<UGameplayEffect> EffectClass, FGameplayTag Tag, float Amount);

    UPROPERTY()
    TObjectPtr<UCombatAttributeSet> CombatAttributes;

    FActiveGameplayEffectHandle DodgeInvincibilityHandle;
    float HitInvincibilityDuration = 0.0f;
    bool bCombatInitialized = false;
    bool bDeathNotified = false;
    bool bUpdatingInitialHealth = false;
};
