// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "RangeCheck.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API URangeCheck : public UBTService
{
	GENERATED_BODY()
public:
	URangeCheck();

protected:
	// 서비스는 TickNode를 통해 주기적으로 실행
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
	struct FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	struct FBlackboardKeySelector IsInRangeKey;
};
