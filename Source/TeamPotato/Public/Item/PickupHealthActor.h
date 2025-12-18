// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/PickupActor.h"
#include "PickupHealthActor.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API APickupHealthActor : public APickupActor
{
	GENERATED_BODY()
	
protected:
    virtual void OnPickup(AActor* InPlayer) override;

    void Heal(AActor* InPlayer);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    float HealAmount = 20.0f;
};
