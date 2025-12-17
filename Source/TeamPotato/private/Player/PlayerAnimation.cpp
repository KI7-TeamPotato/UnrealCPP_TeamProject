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

        if (PlayerWeapon == EWeaponType::Sword)
        {
            if (MainPlayer->GetLastInput() == EMovingDirection::Front)
            {
                MainPlayer->PlayDodgeMontage_Front_Sword();
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::FrontLeft)
            {
                MainPlayer->RotatePlayer(false);            //몸을 왼쪽으로 회전
                MainPlayer->PlayDodgeMontage_Front_Sword();
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::FrontRight)
            {
                MainPlayer->RotatePlayer(true);             //몸을 오른쪽으로 회전
                MainPlayer->PlayDodgeMontage_Front_Sword();
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::Left)
            {
                MainPlayer->PlayDodgeMontage_Left_Sword();
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::Right)
            {
                MainPlayer->PlayDodgeMontage_Right_Sword();
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::Back)
            {
                MainPlayer->PlayDodgeMontage_Back_Sword();
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::BackRight)
            {
                MainPlayer->RotatePlayer(false);            //몸을 왼쪽으로 회전
                MainPlayer->PlayDodgeMontage_Back_Sword();
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::BackLeft)
            {
                MainPlayer->RotatePlayer(true);             //몸을 오른쪽으로 회전
                MainPlayer->PlayDodgeMontage_Back_Sword();
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("UPlayerAnimation::UPlayerAnimation | Wrong Saved Input"));
            }
        }
        else if (PlayerWeapon == EWeaponType::Gun)
        {
            if (MainPlayer->GetLastInput() == EMovingDirection::Front)
            {
                MainPlayer->PlayDodgeMontage_Front_Sword();
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::FrontLeft)
            {
                MainPlayer->RotatePlayer(false);
                MainPlayer->PlayDodgeMontage_Front_Sword();
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::FrontRight)
            {
                MainPlayer->RotatePlayer(true);
                MainPlayer->PlayDodgeMontage_Front_Sword();
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::Left)
            {
                UE_LOG(LogTemp, Log, TEXT("Play Left Dash"));
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::Right)
            {
                UE_LOG(LogTemp, Log, TEXT("Play Right Dash"));
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::Back)
            {
                UE_LOG(LogTemp, Log, TEXT("Play Back Dash"));
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::BackRight)
            {
                UE_LOG(LogTemp, Log, TEXT("Play BackRight Dash"));
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::BackLeft)
            {
                UE_LOG(LogTemp, Log, TEXT("Play BackLeft Dash"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("UPlayerAnimation::UPlayerAnimation | Wrong Saved Input"));
            }
        }
        else
        {
            if (MainPlayer->GetLastInput() == EMovingDirection::Front)
            {
                MainPlayer->PlayDodgeMontage_Front_Sword();
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::FrontLeft)
            {
                UE_LOG(LogTemp, Log, TEXT("Play FrontLeft Roll"));
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::FrontRight)
            {
                UE_LOG(LogTemp, Log, TEXT("Play FrontRight Roll"));
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::Left)
            {
                UE_LOG(LogTemp, Log, TEXT("Play Left Dash"));
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::Right)
            {
                UE_LOG(LogTemp, Log, TEXT("Play Right Dash"));
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::Back)
            {
                UE_LOG(LogTemp, Log, TEXT("Play Back Dash"));
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::BackRight)
            {
                UE_LOG(LogTemp, Log, TEXT("Play BackRight Dash"));
            }
            else if (MainPlayer->GetLastInput() == EMovingDirection::BackLeft)
            {
                UE_LOG(LogTemp, Log, TEXT("Play BackLeft Dash"));
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("UPlayerAnimation::UPlayerAnimation | Wrong Saved Input"));
            }
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
			MainPlayer->PlayAttackMontage_Sword();
		}
		else if (PlayerWeapon == EWeaponType::Gun)
		{
			MainPlayer->PlayAttackMontage_Gun();
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("No Weapon Avaliable"));
		}
	}
}
