// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/TestWeapon.h"
#include "Action/PlayerAttack.h"



void ATestWeapon::BeginPlay()
{
	Super::BeginPlay();
	ExecutePlayerAttack = CreateDefaultSubobject<APlayerAttack>(TEXT("PlayerAttackAction"));
}

void ATestWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATestWeapon::Attack(ATestCharacter* OwningPlayer)
{
	ExecutePlayerAttack->PlayerAttackAction(AttackPower, OwningPlayer, WeaponType);
}
