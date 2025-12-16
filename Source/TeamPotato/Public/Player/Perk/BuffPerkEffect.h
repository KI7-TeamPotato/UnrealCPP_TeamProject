// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PerkEffectBase.h"
#include "BuffPerkEffect.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UBuffPerkEffect : public UPerkEffectBase
{
	GENERATED_BODY()
	
public:
	virtual void OnEquip(AActor* OwnerCharacter) override;

	virtual void OnUnequip(AActor* OwnerCharacter) override;

protected:
	UPROPERTY(EditaNywhere, BlueprintReadOnly, Category = "Buff Perk Effect")
	float BuffAmount = 10.f;
};
