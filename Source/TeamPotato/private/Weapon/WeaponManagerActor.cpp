// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponManagerActor.h"
#include "Player/TestCharacter.h"
#include "Weapon/TestWeapon.h"


AWeaponManagerActor::AWeaponManagerActor()
{
	TestWeapon = CreateDefaultSubobject<ATestWeapon>(TEXT("TestWeapon"));
	TestWeapon->bIsActivated = true;		//무기 선택시 활성화할 함수. 임시로 일단 활성화하게 해둠
	
}

void AWeaponManagerActor::ActivateWeapon()
{
}

void AWeaponManagerActor::WeaponAttack(ATestCharacter* Player)
{
	/*if (TestWeapon->bIsActivated)
	{
	}*/

		TestWeapon->Attack(Player);
}
