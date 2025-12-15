// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoomBase.h"
#include "DungeonRoom9.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API ADungeonRoom9 : public ARoomBase
{
	GENERATED_BODY()
public:
	ADungeonRoom9();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ClosingWall1;
};
