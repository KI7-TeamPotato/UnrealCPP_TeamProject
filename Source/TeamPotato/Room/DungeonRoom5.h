// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoomBase.h"
#include "DungeonRoom5.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API ADungeonRoom5 : public ARoomBase
{
	GENERATED_BODY()
public:
	ADungeonRoom5();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UArrowComponent>  ExitArrow1;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ClosingWall1;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ClosingWall2;
};
