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
		/*PlayerWeapon = MainPlayer->GetPlayerActivatedWeapon();
		if (PlayerWeapon == EWeaponType::Sword)
		{
		}
		MainPlayer->PlaySwordRollMontage();
		else if (PlayerWeapon == EWeaponType::Gun)
		{
			UE_LOG(LogTemp, Log, TEXT("Roll with Gun"));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("No Weapon Avaliable"));
		}*/

        PlayerWeapon = MainPlayer->GetPlayerActivatedWeapon();

        if (PlayerWeapon == EWeaponType::Sword)
        {
            if (MainPlayer->GetPlayerDirection() == EMovingDirection::Front)
            {
                MainPlayer->PlayDodgeMontage_Front_Sword();
            }
            else if (MainPlayer->GetPlayerDirection() == EMovingDirection::FrontLeft)
            {
                MainPlayer->RotatePlayer(EMovingDirection::Left);            //몸을 왼쪽으로 회전
                MainPlayer->PlayDodgeMontage_Front_Sword();
            }
            else if (MainPlayer->GetPlayerDirection() == EMovingDirection::FrontRight)
            {
                MainPlayer->RotatePlayer(EMovingDirection::Right);             //몸을 오른쪽으로 회전
                MainPlayer->PlayDodgeMontage_Front_Sword();
            }
            else if (MainPlayer->GetPlayerDirection() == EMovingDirection::Left)
            {
                MainPlayer->PlayDodgeMontage_Left_Sword();
            }
            else if (MainPlayer->GetPlayerDirection() == EMovingDirection::Right)
            {
                MainPlayer->PlayDodgeMontage_Right_Sword();
            }
            else if (MainPlayer->GetPlayerDirection() == EMovingDirection::Back)
            {
                MainPlayer->PlayDodgeMontage_Back_Sword();
            }
            else if (MainPlayer->GetPlayerDirection() == EMovingDirection::BackRight)
            {
                MainPlayer->RotatePlayer(EMovingDirection::Left);            //몸을 왼쪽으로 회전
                MainPlayer->PlayDodgeMontage_Back_Sword();
            }
            else if (MainPlayer->GetPlayerDirection() == EMovingDirection::BackLeft)
            {
                MainPlayer->RotatePlayer(EMovingDirection::Right);             //몸을 오른쪽으로 회전
                MainPlayer->PlayDodgeMontage_Back_Sword();
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("UPlayerAnimation::UPlayerAnimation | Wrong Saved Input"));
            }
        }
        else if (PlayerWeapon == EWeaponType::Gun)
        {
            if (MainPlayer->GetPlayerDirection() == EMovingDirection::Front)
            {
                MainPlayer->PlayDodgeMontage_Front_Gun();
            }
            else if (MainPlayer->GetPlayerDirection() == EMovingDirection::FrontLeft)
            {
                MainPlayer->RotatePlayer(EMovingDirection::Left);        //몸을 왼쪽으로 회전
                MainPlayer->PlayDodgeMontage_Front_Gun();
            }
            else if (MainPlayer->GetPlayerDirection() == EMovingDirection::FrontRight)
            {
                MainPlayer->RotatePlayer(EMovingDirection::Right);         //몸을 오른쪽으로 회전
                MainPlayer->PlayDodgeMontage_Front_Gun();
            }
            else if (MainPlayer->GetPlayerDirection() == EMovingDirection::Left)
            {
                MainPlayer->PlayDodgeMontage_Left_Gun();
            }
            else if (MainPlayer->GetPlayerDirection() == EMovingDirection::Right)
            {
                MainPlayer->PlayDodgeMontage_Right_Gun();
            }
            else if (MainPlayer->GetPlayerDirection() == EMovingDirection::Back)
            {
                MainPlayer->PlayDodgeMontage_Back_Gun();
            }
            else if (MainPlayer->GetPlayerDirection() == EMovingDirection::BackRight)
            {
                MainPlayer->RotatePlayer(EMovingDirection::Left);         //몸을 왼쪽으로 회전
                MainPlayer->PlayDodgeMontage_Back_Gun();
            }
            else if (MainPlayer->GetPlayerDirection() == EMovingDirection::BackLeft)
            {
                MainPlayer->RotatePlayer(EMovingDirection::Right);         //몸을 오른쪽으로 회전
                MainPlayer->PlayDodgeMontage_Back_Gun();
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("UPlayerAnimation::UPlayerAnimation | Wrong Saved Input"));
            }
        }
        else
        {
             UE_LOG(LogTemp, Error, TEXT("UPlayerAnimation::UPlayerAnimation | Wrong Saved Input"));
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
			MainPlayer->PlayGunShootingMontage();
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("No Weapon Avaliable"));
		}
	}
}
