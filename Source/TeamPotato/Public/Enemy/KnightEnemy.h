// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyCharacter.h"
#include "KnightEnemy.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API AKnightEnemy : public AEnemyCharacter
{
	GENERATED_BODY()
public:
    AKnightEnemy();

    //인터페이스 함수 오버라이드, EnumBase에 있는 열거형에 따라 상태별 속도 설정
    virtual float SetMovementSpeed_Implementation(EEnemySpeed State) override;

};
