// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "FocusPlayer.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UFocusPlayer : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UFocusPlayer();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
public:
	// 회전 속도
	UPROPERTY(EditAnywhere, Category = "AI")
	float TurnSpeed = 5.0f;

	// 타겟 키
	UPROPERTY(EditAnywhere, Category = "AI")
	struct FBlackboardKeySelector TargetKey;
};
