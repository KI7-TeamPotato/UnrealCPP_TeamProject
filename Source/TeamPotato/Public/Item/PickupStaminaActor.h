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
    
public:
    APickupStaminaActor();
	
protected:
    virtual void UseItem(AActor* InPlayer) override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    float StaminaAmount = 10.0f;
};
