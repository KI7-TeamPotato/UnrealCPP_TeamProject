// Fill out your copyright notice in the Description page of Project Settings.


#include "Survice/RangeCheck.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyCharacter.h"

URangeCheck::URangeCheck()
{
	NodeName = "RangeCheck";
	Interval = 0.5f; // 0.5초마다 검사 (성능 최적화)
	RandomDeviation = 0.1f;
}

void URangeCheck::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn) return;

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return;

	AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (Target)
	{
		float Distance = ControllingPawn->GetDistanceTo(Target);

		// 거리가 범위 안인지 확인하여 블랙보드에 저장 (true/false)
		bool bInRange = (Distance <= AttackRange);
		Blackboard->SetValueAsBool(IsInRangeKey.SelectedKeyName, bInRange);
	}
}
