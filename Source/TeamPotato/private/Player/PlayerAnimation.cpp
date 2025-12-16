// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerAnimation.h"
#include "Player/TestCharacter.h"
#include "Weapon/WeaponManagerActor.h"
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
		EWeaponType playerWeapon = MainPlayer->GetPlayerActivatedWeapon();
		if (playerWeapon == EWeaponType::Sword)
		{
			MainPlayer->PlaySwordRollMontage();
		}
		else if (playerWeapon == EWeaponType::Gun)
		{
			UE_LOG(LogTemp, Log, TEXT("GunRoll"))
		}
		else		//이 사이에 다른 무기들 더 넣으면 됨
		{
			UE_LOG(LogTemp, Log, TEXT("Roll with Nothing"));	
		}
	}
}

void UPlayerAnimation::PlayAttackAnimation()
{
	if (MainPlayer.IsValid())
	{
		EWeaponType playerWeapon = MainPlayer->GetPlayerActivatedWeapon();
		if (playerWeapon == EWeaponType::Sword)
		{
			MainPlayer->PlaySwordAttackMontage();
		}
		else if(playerWeapon == EWeaponType::Gun)
		{
			UE_LOG(LogTemp, Log, TEXT("GunShot"));
		}
		else		//이 사이에 다른 무기들 더 넣으면 됨
			return;
	}
}
