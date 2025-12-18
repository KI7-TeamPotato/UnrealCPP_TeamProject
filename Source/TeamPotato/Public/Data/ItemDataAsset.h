// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Item/PickupActor.h"
#include "ItemDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UItemDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
    // 드랍 아이템 클래스
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actor Classes")
    TSubclassOf<APickupActor> PickupItemClass;

    // 아이템 가격
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Data")
    int32 ItemPrice = 0;
};
