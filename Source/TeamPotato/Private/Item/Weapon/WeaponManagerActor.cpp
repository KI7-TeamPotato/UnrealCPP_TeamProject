// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/WeaponManagerActor.h"
#include "Player/TestCharacter.h"
#include "Item/Weapon/TestWeapon.h"


AWeaponManagerActor::AWeaponManagerActor()
{
	TestWeaponClass = ATestWeapon::StaticClass();
}

void AWeaponManagerActor::BeginPlay()
{
	Super::BeginPlay();

	if (!TestWeaponClass || !GetWorld())
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	TestWeapon = GetWorld()->SpawnActor<ATestWeapon>(TestWeaponClass, GetActorTransform(), SpawnParameters);
	if (TestWeapon)
	{
		TestWeapon->bIsActivated = true;
	}
}

void AWeaponManagerActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(TestWeapon))
	{
		TestWeapon->Destroy();
		TestWeapon = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void AWeaponManagerActor::ActivateWeapon()
{
}

void AWeaponManagerActor::WeaponAttack(ATestCharacter* Player)
{
	/*if (TestWeapon->bIsActivated)
	{
	}*/
	if (IsValid(TestWeapon) && IsValid(Player) && TestWeapon->bIsActivated)
	{
		TestWeapon->Attack(Player);
	}
}
