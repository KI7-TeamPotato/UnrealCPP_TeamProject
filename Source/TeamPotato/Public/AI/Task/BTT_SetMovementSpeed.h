// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "IntetFace/EnemyInterface.h"
#include "BTT_SetMovementSpeed.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UBTT_SetMovementSpeed : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_SetMovementSpeed();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
protected:
	//얘만 인터페이스랑 연동해서 씀
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	EEnemySpeed MovementState;
};
