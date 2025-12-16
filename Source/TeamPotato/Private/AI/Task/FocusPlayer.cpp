// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/FocusPlayer.h"
#include "AIController.h"
#include "Enemy/EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UFocusPlayer::UFocusPlayer()
{
	NodeName = "PlayerFocus";
    bNotifyTick = true;
}

EBTNodeResult::Type UFocusPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    return EBTNodeResult::InProgress;
}

void UFocusPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIC ? AIC->GetPawn() : nullptr);

    UObject* TargetObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetKey.SelectedKeyName);
    AActor* TargetActor = Cast<AActor>(TargetObject);

    Enemy->PlayerFocus(TargetActor, DeltaSeconds, TurnSpeed);

    if (Enemy->PlayerFocus(TargetActor, DeltaSeconds, TurnSpeed))
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
