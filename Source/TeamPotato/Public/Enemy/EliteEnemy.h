// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyCharacter.h"
#include "Data/EnumBase.h"
#include "Intetface/EnemyInterface.h"
#include "EliteEnemy.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API AEliteEnemy : public AEnemyCharacter
{
	GENERATED_BODY()

public:
    AEliteEnemy();

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void GrabAttack();
protected:
    virtual void BeginPlay() override;

public:
    // 부모의 공격 함수를 오버라이드하여 특수 패턴 추가
    virtual void DefaultAttack() override;

    // 특수 공격 실행 함수
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void PerformSpecialAttack();

    virtual float SetMovementSpeed_Implementation(EEnemySpeed State) override;
protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elite Stats")
    float HealthMultiplier = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Elite Stats")
    float ScaleMultiplier = 1.2f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    UAnimMontage* SpecialAttackMontage;

};
