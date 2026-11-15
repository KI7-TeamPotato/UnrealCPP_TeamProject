// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/FocusPlayer.h"
#include "AIController.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/LastBoss.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

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

    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
    if (!IsValid(Enemy) || !Blackboard)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
    if (Enemy->IsA<ALastBoss>())
    {
        // Character selection replaces the player's pawn during the encounter.
        ACharacter* CurrentPlayer = UGameplayStatics::GetPlayerCharacter(Enemy, 0);
        if (TargetActor != CurrentPlayer)
        {
            Blackboard->SetValueAsObject(TargetKey.SelectedKeyName, CurrentPlayer);
            TargetActor = CurrentPlayer;
        }
    }

    Enemy->PlayerFocus(TargetActor, DeltaSeconds, TurnSpeed);
    if (Enemy->PlayerFocus(TargetActor, DeltaSeconds, TurnSpeed))
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
