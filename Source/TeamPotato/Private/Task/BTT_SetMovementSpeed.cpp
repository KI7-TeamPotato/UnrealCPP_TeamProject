 // Fill out your copyright notice in the Description page of Project Settings.


#include "Task/BTT_SetMovementSpeed.h"
#include "AIController.h"
#include "Enemy/EnemyCharacter.h"

UBTT_SetMovementSpeed::UBTT_SetMovementSpeed()
{
    NodeName = "Set Movement Speed";

    MovementState = EEnemySpeed::Walking;

}

EBTNodeResult::Type UBTT_SetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC)
    {
        return EBTNodeResult::Failed;
    }
    APawn* ControlledPawn = AIC->GetPawn();
    if (!ControlledPawn)
    {
        return EBTNodeResult::Failed;
    }
    if (ControlledPawn->Implements<UEnemyInterface>())
    {
        IEnemyInterface::Execute_SetMovementSpeed(ControlledPawn, MovementState);

        return EBTNodeResult::Succeeded;
    }

    // 인터페이스가 없는 캐릭터라면 실패 처리
    return EBTNodeResult::Failed;
}
