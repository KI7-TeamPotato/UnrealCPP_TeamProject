// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShopTableActor.generated.h"

UCLASS()
class TEAMPOTATO_API AShopTableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShopTableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
    // 테이블 매쉬
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
    TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

    // 아이템 스폰 포인트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
    TObjectPtr<USceneComponent> SpawnPoint = nullptr;
};
