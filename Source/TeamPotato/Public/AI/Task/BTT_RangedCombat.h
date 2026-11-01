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

private:
    ERangedCombatAction Action = ERangedCombatAction::Attack;
    double NextDecisionTime = 0.0;
    double NextRepathTime = 0.0;
    FVector LastTargetLocation = FVector::ZeroVector;
    void Cleanup(UBehaviorTreeComponent& OwnerComp);
};
