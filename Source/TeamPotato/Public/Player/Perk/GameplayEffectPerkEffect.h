#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Player/Perk/PerkEffectBase.h"
#include "GameplayEffectPerkEffect.generated.h"

class UGameplayEffect;
class UCombatAbilitySystemComponent;

// One persistent, editor-authored GE per owned perk. Removing the perk removes only its own effect.
UCLASS(EditInlineNew)
class TEAMPOTATO_API UGameplayEffectPerkEffect : public UPerkEffectBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay Effect")
    TSubclassOf<UGameplayEffect> EffectClass;

    virtual bool OnEquip(AActor* OwnerCharacter) override;
    virtual void OnUnequip(AActor* OwnerCharacter) override;

private:
    TWeakObjectPtr<UCombatAbilitySystemComponent> AppliedTo;
    FActiveGameplayEffectHandle EffectHandle;
};
