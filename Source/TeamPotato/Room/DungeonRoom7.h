// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoomBase.h"
#include "DungeonRoom7.generated.h"

class AWeaponBoxActor;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API ADungeonRoom7 : public ARoomBase
{
	GENERATED_BODY()
public:
	ADungeonRoom7();

    UFUNCTION()
    void SpawnTreasureBox();
protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

    UPROPERTY()
    AWeaponBoxActor* SpawnedBoxInstance; // 스폰된 상자를 저장할 변수
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UArrowComponent>  TreasureBoxSpawnPoint;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UArrowComponent>  ExitArrow1;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UArrowComponent>  ExitArrow2;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ClosingWall1;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ClosingWall2;

    UPROPERTY(EditAnywhere, Category = "Treasure")
    TArray<TSubclassOf<AWeaponBoxActor>> TreasureBox;
};
