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

        ////y값 1증가(블렌드 스페이스에서 0~2값 사용), 대각선 방향 보정을 위해 x값 * 0,5 * -y값. smoothstep을 위해 0~1사이로 만들어야 해서 2로 나눔
        //float playerYdirectValue = (PlayerMovingDirection.Y + 1.0f + PlayerMovingDirection.X * 0.5f * (-(PlayerMovingDirection.Y)))/2;     
        //float smoothPlayerDireciton = (playerYdirectValue * playerYdirectValue * (3.0f - 2.0f * playerYdirectValue))*2;                 //smoothstep, 0~2사이 값으로 하기 위해 2곱함

        float playerYdirectValue = PlayerMovingDirection.Y + 1.0f;

        PlayerSideDirection = std::clamp(playerYdirectValue, 0.0f, 2.0f);            //혹시 모르니 0~2사이를 벗어나지 않도록 보정


        UE_LOG(LogTemp, Log, TEXT("Y : %f"), PlayerSideDirection);

        //앞으로 가는지 뒤로 가는지 확인
        if (PlayerMovingDirection.X > 0)
            bIsPlayerGoingFront = true;
        else if (PlayerMovingDirection.X < 0)
            bIsPlayerGoingFront = false;
	}
}
