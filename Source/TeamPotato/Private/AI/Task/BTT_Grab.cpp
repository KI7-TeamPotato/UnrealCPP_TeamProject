// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTT_Grab.h"
#include "AIController.h"
#include "Enemy/EliteEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_Grab::UBTT_Grab()
{
    //노드에 표시할 이름 (다른 태스크도 같음 이름만 다름)
    NodeName = "Grab";
}

EBTNodeResult::Type UBTT_Grab::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) return EBTNodeResult::Failed;

    AEliteEnemy* Enemy = Cast<AEliteEnemy>(AIC->GetPawn());
    if (!Enemy) return EBTNodeResult::Failed;

    Enemy->GrabAttack();

    return EBTNodeResult::Succeeded;
}
