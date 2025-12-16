// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerAnimation.h"
#include "Player/TestCharacter.h"
#include "Item/Weapon/WeaponManagerActor.h"
#include "Animation/AnimInstance.h"

// Sets default values for this component's properties
UPlayerAnimation::UPlayerAnimation()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UPlayerAnimation::BeginPlay()
{
	Super::BeginPlay();

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	MainPlayer = Cast<ATestCharacter>(OwnerPawn);

}

void UPlayerAnimation::PlayRollMontage()
{
	if (MainPlayer.IsValid())
	{
		PlayerWeapon = MainPlayer->GetPlayerActivatedWeapon();
		if (PlayerWeapon == EWeaponType::Sword)
		{
			MainPlayer->PlaySwordRollMontage();
		}
		else if (PlayerWeapon == EWeaponType::Gun)
		{
			UE_LOG(LogTemp, Log, TEXT("Roll with Gun"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("No Weapon Avaliable"));
		}
	}
}

void UPlayerAnimation::PlayAttackAnimation()
{
	if (MainPlayer.IsValid())
	{
		PlayerWeapon = MainPlayer->GetPlayerActivatedWeapon();
		if (PlayerWeapon == EWeaponType::Sword)
		{
			MainPlayer->PlaySwordAttackMontage();
		}
		else if (PlayerWeapon == EWeaponType::Gun)
		{
			UE_LOG(LogTemp, Log, TEXT("Shoot Gun"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("No Weapon Avaliable"));
		}
	}
}
