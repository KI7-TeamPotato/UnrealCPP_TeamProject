// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_WieldWeapon.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UBTT_WieldWeapon : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_WieldWeapon();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
