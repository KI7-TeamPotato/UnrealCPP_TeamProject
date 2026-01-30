// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EliteArcher.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIController.h" 
#include "BrainComponent.h"

AEliteArcher::AEliteArcher()
{
}

float AEliteArcher::SetMovementSpeed_Implementation(EEnemySpeed State)
{
    float TargetSpeed = 0.0f;

    //EEnemySpeed에 따라서 속도 조절
    switch (State)
    {
    case EEnemySpeed::Idle:
        TargetSpeed = 0.0f;
        break;
    case EEnemySpeed::Walking:
        TargetSpeed = 300.0f;
        break;
    case EEnemySpeed::Jumping:
        TargetSpeed = 300.0f;
        break;
    case EEnemySpeed::Sprint:
        TargetSpeed = 300.0f;
        break;
    default:
        TargetSpeed = 300.0f; // 기본값
        break;
    }

    // 실제 이동 속도 적용
    GetCharacterMovement()->MaxWalkSpeed = TargetSpeed;

    // 변경된 속도 반환
    return TargetSpeed;
}
