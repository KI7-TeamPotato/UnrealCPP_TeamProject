#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_RangedCombat.generated.h"

class AArcherEnemy;

UENUM(BlueprintType)
enum class ERangedCombatAction : uint8 { Attack, Approach, Retreat };

// This task owns movement and firing for the entire ranged combat branch.
UCLASS()
class TEAMPOTATO_API UBTT_RangedCombat : public UBTTask_BlackboardBase
{
    GENERATED_BODY()
public:
    UBTT_RangedCombat();
    static ERangedCombatAction SelectAction(const AArcherEnemy& Enemy, float Distance, bool bHasLOS, ERangedCombatAction Previous);
    static bool FindMoveGoal(AArcherEnemy& Enemy, AActor& Target, ERangedCombatAction Action, FVector& OutGoal);

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

private:
    ERangedCombatAction Action = ERangedCombatAction::Attack;
    double NextDecisionTime = 0.0;
    double NextRepathTime = 0.0;
    FVector LastTargetLocation = FVector::ZeroVector;
    TWeakObjectPtr<AArcherEnemy> FacingEnemy;
    bool bSavedControllerYaw = false;
    bool bSavedOrientToMovement = false;
    bool bSavedControllerDesiredRotation = false;
    void SetMovementFacing(AArcherEnemy& Enemy, bool bMoving);
    void RestoreFacing();
    void Cleanup(UBehaviorTreeComponent& OwnerComp);
};
