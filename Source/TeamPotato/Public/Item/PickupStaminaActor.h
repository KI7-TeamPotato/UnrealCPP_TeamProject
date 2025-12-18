// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/PickupActor.h"
#include "PickupStaminaActor.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API APickupStaminaActor : public APickupActor
{
	GENERATED_BODY()
	
protected:
    virtual void OnPickup(AActor* InPlayer) override;

    void AddStamina(AActor* InPlayer);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    float StaminaAmount = 10.0f;
};
