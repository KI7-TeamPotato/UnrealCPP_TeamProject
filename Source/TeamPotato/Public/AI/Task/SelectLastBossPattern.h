#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SelectLastBossPattern.generated.h"

class ALastBoss;

UCLASS(Blueprintable)
class TEAMPOTATO_API USelectLastBossPattern : public UBTTaskNode
{
    GENERATED_BODY()

public:
    USelectLastBossPattern();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
    UFUNCTION()
    void HandlePatternFinished();
    void UnbindPattern();

    TWeakObjectPtr<ALastBoss> ActiveBoss;
    TWeakObjectPtr<UBehaviorTreeComponent> ActiveTree;
    int32 PreviousPattern = INDEX_NONE;
    bool bStartingPattern = false;
};
