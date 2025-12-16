// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTT_WieldWeapon.h"
#include "AIController.h"
#include "Enemy/EnemyCharacter.h"


UBTT_WieldWeapon::UBTT_WieldWeapon()
{
    NodeName = "Wield Weapon";
}

EBTNodeResult::Type UBTT_WieldWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) return EBTNodeResult::Failed;

    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(AIC->GetPawn());
    if (!Enemy) return EBTNodeResult::Failed;

    // 캐릭터의 WieldSword 함수 호출
    Enemy->WieldWeapon();

    return EBTNodeResult::Succeeded;
}
