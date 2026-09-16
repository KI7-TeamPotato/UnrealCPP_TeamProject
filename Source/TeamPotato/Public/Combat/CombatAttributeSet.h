#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CombatAttributeSet.generated.h"

#define COMBAT_ATTRIBUTE_ACCESSORS(PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UCombatAttributeSet, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class TEAMPOTATO_API UCombatAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UCombatAttributeSet();

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    FGameplayAttributeData Health;
    COMBAT_ATTRIBUTE_ACCESSORS(Health)

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    FGameplayAttributeData MaxHealth;
    COMBAT_ATTRIBUTE_ACCESSORS(MaxHealth)

    // A transient, positive damage amount. Consumed and reset for every execution.
    UPROPERTY()
    FGameplayAttributeData IncomingDamage;
    COMBAT_ATTRIBUTE_ACCESSORS(IncomingDamage)

    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
    virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

private:
    void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;
};

#undef COMBAT_ATTRIBUTE_ACCESSORS
