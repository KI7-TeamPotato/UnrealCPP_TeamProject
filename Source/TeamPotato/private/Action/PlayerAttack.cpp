// Fill out your copyright notice in the Description page of Project Settings.


#include "Action/PlayerAttack.h"
#include "Player/TestCharacter.h"
#include "Weapon/TestWeapon.h"

// Sets default values
APlayerAttack::APlayerAttack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void APlayerAttack::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerAttack::PlayerAttackAction(float InDamage, ATestCharacter* MainPlayer, EWeaponType WeaponType)
{
	if (WeaponType == EWeaponType::Sword)
	{
		MainPlayer->PlaySwordAttackMontage();
	}
}

