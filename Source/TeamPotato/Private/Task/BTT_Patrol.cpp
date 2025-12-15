// Fill out your copyright notice in the Description page of Project Settings.


#include "Task/BTT_Patrol.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_Patrol::UBTT_Patrol()
{
    NodeName = "Find Patrol Pos";
}

EBTNodeResult::Type UBTT_Patrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) return EBTNodeResult::Failed;

	APawn* ControllingPawn = AIC->GetPawn();
	if (!ControllingPawn) return EBTNodeResult::Failed;

	// 네비게이션 시스템 가져오기
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem) return EBTNodeResult::Failed;

	FNavLocation NextPatrolPos;

	// 현재 폰의 위치를 기준으로 반경(PatrolRadius) 내에서 랜덤한 위치 찾기
	if (NavSystem->GetRandomPointInNavigableRadius(ControllingPawn->GetActorLocation(), PatrolRadius, NextPatrolPos))
	{
		// 찾은 위치를 블랙보드에 저장
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(PatrolPosKey.SelectedKeyName, NextPatrolPos.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
