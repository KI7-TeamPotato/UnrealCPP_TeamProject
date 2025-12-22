// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/PlayerAnimInstance.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Player/TestCharacter.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	APawn* ownerPawn = TryGetPawnOwner();
	if (ownerPawn)
	{
		OwnerMovementComponent = ownerPawn->GetMovementComponent();
        MainPlayer = Cast<ATestCharacter>(ownerPawn);
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwnerMovementComponent.IsValid())
	{
        //플레이어가 움직이는 속도 구하기
		Speed = OwnerMovementComponent.Get()->Velocity.Size();

        //위아래 시야각 구하기
        SightDegree = MainPlayer->GetSightDegree();

        //활성화 된 무기 판별
        EWeaponType ActivatedWeapon = EWeaponType::None;
        ActivatedWeapon =  MainPlayer->GetPlayerActivatedWeapon();
        if (ActivatedWeapon == EWeaponType::Sword)
        {
            bIsGunActivated = false;
            bIsSwordActivated = true;
        }
        else if (ActivatedWeapon == EWeaponType::Gun)
        {
            bIsSwordActivated = false;
            bIsGunActivated = true;
        }

        //플레이어의 움직이는 방향 구하기
        PlayerMovingDirection = MainPlayer->GetLastInput();
        if ((PlayerMovingDirection == EMovingDirection::FrontLeft) || (PlayerMovingDirection == EMovingDirection::FrontRight))
            PlayerMovingDirection = EMovingDirection::Front;
        else if ((PlayerMovingDirection == EMovingDirection::BackLeft) || (PlayerMovingDirection == EMovingDirection::BackRight))
            PlayerMovingDirection = EMovingDirection::Back;
	}
}
