// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClosingWall.generated.h"

UCLASS()
class TEAMPOTATO_API AClosingWall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AClosingWall();

	FVector GetLoc() {
		return WallLocationOffset;
	}

	FRotator GetRot() {
		return WallRotationOffset;
	}
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ClosingWall;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Wall")
	FVector WallLocationOffset = FVector(0.0f, -55.0f, 15.0f); // 기본값 설정

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Wall")
	FRotator WallRotationOffset = FRotator(0.0f, 0.0f, 0.0f); // 기본값 설정
};
