// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DungeonRoom8.h"
#include "DungeonRoom8_S2.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API ADungeonRoom8_S2 : public ADungeonRoom8
{
	GENERATED_BODY()

public:
    ADungeonRoom8_S2();

    UPROPERTY(EditAnywhere, Category = "Treasure")
    TSubclassOf<AShopTableActor> ShopTable_HP2;

    UPROPERTY(EditAnywhere, Category = "Treasure")
    TSubclassOf<AShopTableActor> ShopTable_HP3;
};
