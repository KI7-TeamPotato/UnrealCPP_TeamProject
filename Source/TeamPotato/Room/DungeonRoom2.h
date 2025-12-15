// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoomBase.h"
#include "DungeonRoom2.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API ADungeonRoom2 : public ARoomBase
{
	GENERATED_BODY()
public:
	ADungeonRoom2();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UArrowComponent>  ExitArrow1;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UArrowComponent> ExitArrow2;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ClosingWall;
};
