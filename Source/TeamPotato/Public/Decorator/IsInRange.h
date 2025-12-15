// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "IsInRange.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UIsInRange : public UBTDecorator
{
	GENERATED_BODY()
public:
	UIsInRange();

protected:
	// 조건 계산 함수 (True면 노드 실행, False면 실행 안 함)
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	// 공격 사거리
	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackRange = 150.0f;

	// 타겟 키 (블랙보드)
	UPROPERTY(EditAnywhere, Category = "AI")
	struct FBlackboardKeySelector TargetKey;
};
