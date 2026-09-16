#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "CombatEffects.generated.h"

class UTargetTagsGameplayEffectComponent;
class UTargetTagRequirementsGameplayEffectComponent;

UCLASS(Abstract)
class TEAMPOTATO_API UCombatGameplayEffect : public UGameplayEffect
{
    GENERATED_BODY()
public:
    UCombatGameplayEffect();
protected:
    void GrantCombatTag(FGameplayTag Tag);
    void BlockDeadTargets();

    UPROPERTY()
    TObjectPtr<UTargetTagsGameplayEffectComponent> GrantedTags;
    UPROPERTY()
    TObjectPtr<UTargetTagRequirementsGameplayEffectComponent> TargetRequirements;
};

// Native defaults make the migration usable without manually creating GE assets.
UCLASS()
class TEAMPOTATO_API UCombatDamageEffect : public UCombatGameplayEffect
{
    GENERATED_BODY()
public:
    UCombatDamageEffect();
};

UCLASS()
class TEAMPOTATO_API UCombatHealEffect : public UCombatGameplayEffect
{
    GENERATED_BODY()
public:
    UCombatHealEffect();
};

UCLASS()
class TEAMPOTATO_API UCombatMaxHealthEffect : public UCombatGameplayEffect
{
    GENERATED_BODY()
public:
    UCombatMaxHealthEffect();
};

UCLASS()
class TEAMPOTATO_API UCombatHitInvincibilityEffect : public UCombatGameplayEffect
{
    GENERATED_BODY()
public:
    UCombatHitInvincibilityEffect();
};

UCLASS()
class TEAMPOTATO_API UCombatDodgeInvincibilityEffect : public UCombatGameplayEffect
{
    GENERATED_BODY()
public:
    UCombatDodgeInvincibilityEffect();
};

UCLASS()
class TEAMPOTATO_API UCombatDeadEffect : public UCombatGameplayEffect
{
    GENERATED_BODY()
public:
    UCombatDeadEffect();
};
