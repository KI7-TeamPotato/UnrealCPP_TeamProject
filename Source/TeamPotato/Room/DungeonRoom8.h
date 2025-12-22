// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoomBase.h"
#include "DungeonRoom8.generated.h"
class AShopTableActor;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API ADungeonRoom8 : public ARoomBase
{
	GENERATED_BODY()
public:
    ADungeonRoom8();

    UFUNCTION()
    void SpawnShopTable();

protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
    UPROPERTY()
    AShopTableActor* ShopTableInstance;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UArrowComponent>  ShopTableSpawnPoint_HP;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UArrowComponent>  ShopTableSpawnPoint_Energy;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UArrowComponent>  ShopTableSpawnPoint_Weapon;

    UPROPERTY(EditAnywhere, Category = "Treasure")
    TSubclassOf<AShopTableActor> ShopTable_HP;

    UPROPERTY(EditAnywhere, Category = "Treasure")
    TSubclassOf<AShopTableActor> ShopTable_Energy;

    UPROPERTY(EditAnywhere, Category = "Treasure")
    TSubclassOf<AShopTableActor> ShopTable_Weapon;
};
