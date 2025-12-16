// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTT_DefaultAttack.h"
#include "AIController.h"
#include "Enemy/EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_DefaultAttack::UBTT_DefaultAttack()
{
    //노드에 표시할 이름 (다른 태스크도 같음 이름만 다름)
    NodeName = "DefaultAttack"; 
}

EBTNodeResult::Type UBTT_DefaultAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) return EBTNodeResult::Failed;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIC->GetPawn());
    if (!Enemy) return EBTNodeResult::Failed;

    Enemy->DefaultAttack();

    return EBTNodeResult::Succeeded;
}
