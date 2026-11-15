#include "AI/Task/SelectLastBossPattern.h"
#include "AIController.h"
#include "Enemy/LastBoss.h"

USelectLastBossPattern::USelectLastBossPattern()
{
    NodeName = TEXT("Select Final Boss Pattern");
    bCreateNodeInstance = true;
}

EBTNodeResult::Type USelectLastBossPattern::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* Controller = OwnerComp.GetAIOwner();
    ALastBoss* Boss = Controller ? Cast<ALastBoss>(Controller->GetPawn()) : nullptr;
    if (!IsValid(Boss) || Boss->GetCombatHealth() <= 0.0f)
    {
        return EBTNodeResult::Failed;
    }

    constexpr int32 PatternCount = 5;
    // A nonzero offset visits each of the four alternatives with equal probability.
    PreviousPattern = PreviousPattern == INDEX_NONE ? FMath::RandHelper(PatternCount)
        : (PreviousPattern + FMath::RandRange(1, PatternCount - 1)) % PatternCount;

    ActiveTree = &OwnerComp;
    ActiveBoss = Boss;
    Boss->OnPatternFinished.AddUniqueDynamic(this, &USelectLastBossPattern::HandlePatternFinished);
    {
        // A montage can fail to start and finish the pattern before ExecuteTask returns.
        TGuardValue<bool> StartingGuard(bStartingPattern, true);
        Boss->ExecutePattern(PreviousPattern);
    }
    return ActiveTree.IsValid() ? EBTNodeResult::InProgress : EBTNodeResult::Succeeded;
}

void USelectLastBossPattern::HandlePatternFinished()
{
    UBehaviorTreeComponent* Tree = ActiveTree.Get();
    UnbindPattern();
    if (Tree && !bStartingPattern)
    {
        FinishLatentTask(*Tree, EBTNodeResult::Succeeded);
    }
}

EBTNodeResult::Type USelectLastBossPattern::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ALastBoss* Boss = ActiveBoss.Get();
    UnbindPattern();
    if (Boss)
    {
        Boss->CancelActivePattern();
    }
    return EBTNodeResult::Aborted;
}

void USelectLastBossPattern::UnbindPattern()
{
    if (ALastBoss* Boss = ActiveBoss.Get())
    {
        Boss->OnPatternFinished.RemoveDynamic(this, &USelectLastBossPattern::HandlePatternFinished);
    }
    ActiveBoss.Reset();
    ActiveTree.Reset();
}
