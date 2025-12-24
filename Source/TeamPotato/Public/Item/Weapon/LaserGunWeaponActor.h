// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/EnumBase.h"
#include "Item/Weapon/WeaponBase.h"
#include "LaserGunWeaponActor.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API ALaserGunWeaponActor : public AWeaponBase
{
	GENERATED_BODY()

public:
    ALaserGunWeaponActor();
	
protected:
    UFUNCTION(BlueprintCallable)
    virtual void Attack(class ATestCharacter* OwningPlayer) override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TSubclassOf<AActor> LaserClass;
};
