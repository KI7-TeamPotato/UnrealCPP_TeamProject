// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Patrol.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UBTT_Patrol : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_Patrol();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	// 에디터에서 이동할 반경 설정
	UPROPERTY(EditAnywhere, Category = "AI")
	float PatrolRadius = 500.0f;

	// 결과를 저장할 블랙보드 키 (Vector 타입이어야 함)
	UPROPERTY(EditAnywhere, Category = "AI")
	struct FBlackboardKeySelector PatrolPosKey;
};
