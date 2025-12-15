// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "TestWeapon.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API ATestWeapon : public AWeaponBase
{
	GENERATED_BODY()
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Attack(class ATestCharacter* OwningPlayer) override;

public:
	static const EWeaponType WeaponType = EWeaponType::Sword;
	const float AttackPower = 1.0f;

private:
	TObjectPtr<class APlayerAttack> ExecutePlayerAttack = nullptr;
};
