// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class UWeaponComponent;

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
	virtual void Attack(class ATestCharacter* OwningPlayer) {};

	inline void SetOwnerComponent(UWeaponComponent* InWeaponComponent) { OwnerWeaponComponent = InWeaponComponent; }

protected:
	UPROPERTY()
	TObjectPtr<UWeaponComponent> OwnerWeaponComponent = nullptr;

public:
	bool bIsActivated = false;
};
