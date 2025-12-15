// Fill out your copyright notice in the Description page of Project Settings.


#include "Decorator/IsInRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/EnemyCharacter.h"

UIsInRange::UIsInRange()
{
	NodeName = "InRange";
	AttackRange = 250.0f;
}

bool UIsInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) return false;

	APawn* ControllingPawn = AIC->GetPawn();
	if (!ControllingPawn) return false;

	// 블랙보드에서 타겟 가져오기
	UObject* TargetObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(TargetObject);

	if (!TargetActor) return false;

	// 거리 계산
	float Distance = ControllingPawn->GetDistanceTo(TargetActor);

	if (Distance <= AttackRange)
	{
		UE_LOG(LogTemp, Log, TEXT("범위 안"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("범위 밖"));

	}

	// 사거리 안에 있으면 True (공격 실행 허가)
	return Distance <= AttackRange;
}
