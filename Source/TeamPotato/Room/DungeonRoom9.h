// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoomBase.h"
#include "Components/ArrowComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DungeonRoom9.generated.h"
class ABossScarecrow;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API ADungeonRoom9 : public ARoomBase
{
	GENERATED_BODY()
public:
	ADungeonRoom9();

    UFUNCTION()
    void SpawnPortal();
protected:
    virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ClosingWall1;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UArrowComponent> PortalPoint;

    UPROPERTY(EditAnywhere, Category = "Components|Portal")
    TSubclassOf<AActor> PortalClass;
};
