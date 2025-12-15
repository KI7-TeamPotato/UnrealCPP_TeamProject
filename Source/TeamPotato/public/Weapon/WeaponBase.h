// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None	UMETA(DisplayName = "None"),
	Sword	UMETA(DisplayName = "Sword"),
	Bow		UMETA(DisplayName = "Bow"),				//테스트용
	Gun		UMETA(DisplayName = "Gun")				//테스트용
};

UCLASS()
class TEAMPOTATO_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Attack(class ATestCharacter* OwningPlayer) {};

public:
	bool bIsActivated = false;
};
