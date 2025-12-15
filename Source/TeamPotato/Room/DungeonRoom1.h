// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoomBase.h"
#include "Component/WaveComponent.h"
#include "DungeonRoom1.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API ADungeonRoom1 : public ARoomBase
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:
	ADungeonRoom1();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWaveComponent> WaveSystemComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UArrowComponent> ExitArrow1;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UArrowComponent>  ExitArrow2;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UArrowComponent>  ExitArrow3;


	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UArrowComponent> SpawnPoint2;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UArrowComponent> SpawnPoint3;


	UFUNCTION()
	void HandleBattleEnd();
};
