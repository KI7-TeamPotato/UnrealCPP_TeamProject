// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon/WeaponBase.h"
#include "WeaponManagerActor.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API AWeaponManagerActor : public AWeaponBase
{
	GENERATED_BODY()
	
public:
	AWeaponManagerActor();

	//무기를 활성화하는 함수
	UFUNCTION()
	void ActivateWeapon();

	//활성화된 무기에 맞춰서 공격을 하게 하는 함수
	UFUNCTION()
	void WeaponAttack(ATestCharacter* Player);

private:
	UPROPERTY()
	TObjectPtr<class ATestWeapon> TestWeapon = nullptr;

};
