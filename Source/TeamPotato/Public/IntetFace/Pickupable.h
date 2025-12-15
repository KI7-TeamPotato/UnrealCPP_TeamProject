// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Pickupable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickupable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TEAMPOTATO_API IPickupable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// 플레이어가 획득 시 호출
	virtual void OnPickup(AActor* InPlayer) = 0;

	// 아이템이 무기인지 확인
	virtual bool ItemInteraction() const { return false; }
};
