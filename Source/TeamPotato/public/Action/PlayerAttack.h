// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponBase.h"
#include "GameFramework/Actor.h"
#include "PlayerAttack.generated.h"

UCLASS()
class TEAMPOTATO_API APlayerAttack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerAttack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	void PlayerAttackAction(float InDamage, class ATestCharacter* MainPlayer, EWeaponType WeaponType);
};
