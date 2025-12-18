// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PerkEffectBase.generated.h"

/**
 * 
 */
 
UCLASS(Abstract, Blueprintable, EditInlineNew)
class TEAMPOTATO_API UPerkEffectBase : public UObject
{
	GENERATED_BODY()
	
public:
	// when the perk is equipped, this function is called
	virtual void OnEquip(AActor* OwnerCharacter) {};

	// when the perk is unequipped, this function is called
	virtual void OnUnequip(AActor* OwnerCharacter) {};

    // 퍽의 변경 가능한 효과 수치를 반환
    float GetAmount() const { return Amount; }

protected:
    UPROPERTY(EditaNywhere, BlueprintReadOnly, Category = "Buff Perk Effect")
    float Amount = 10.f;
};
