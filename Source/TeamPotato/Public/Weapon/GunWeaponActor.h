// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GunWeaponActor.generated.h"

class ABulletActor;
class USkeletalMeshComponent;
class UWeaponComponent;

UCLASS()
class TEAMPOTATO_API AGunWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGunWeaponActor();

	UFUNCTION(BlueprintCallable)
	void Fire();

	inline void SetOwnerComponent(UWeaponComponent* InWeaponComponent) { OwnerWeaponComponent = InWeaponComponent; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> GunMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ABulletActor> BulletClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BulletSpeed = 3000.f;

	UPROPERTY()
	TObjectPtr<UWeaponComponent> OwnerWeaponComponent = nullptr;

};
