// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/EnumBase.h"
#include "Item/Weapon/WeaponBase.h"
#include "GunWeaponActor.generated.h"

class ABulletActor;
class USkeletalMeshComponent;

UCLASS()
class TEAMPOTATO_API AGunWeaponActor : public AWeaponBase	
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGunWeaponActor();

	UFUNCTION(BlueprintCallable)
	virtual void Attack(class ATestCharacter* OwningPlayer) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ABulletActor> BulletClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BulletSpeed = 3000.f;
};
