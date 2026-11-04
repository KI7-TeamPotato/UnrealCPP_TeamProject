#include "AI/Task/BTT_RangedCombat.h"
#include "Enemy/ArcherEnemy.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Navigation/PathFollowingComponent.h"
#include "Combat/CombatFunctionLibrary.h"

namespace
{
    struct FRanges
    {
        float Retreat, Safe, Approach, Max;
        explicit FRanges(const AArcherEnemy& E)
        {
            Retreat = FMath::IsFinite(E.RetreatStartDistance) ? FMath::Max(0.0f,E.RetreatStartDistance) : 450.0f;
            Safe = FMath::IsFinite(E.RetreatStopDistance) ? FMath::Max(Retreat+10.0f,E.RetreatStopDistance) : Retreat+200.0f;
            Max = FMath::IsFinite(E.MaxRangedAttackDistance) ? FMath::Max(Safe+20.0f,E.MaxRangedAttackDistance) : Safe+450.0f;
            Approach = FMath::IsFinite(E.ApproachStopDistance) ? FMath::Clamp(E.ApproachStopDistance,Safe,Max-10.0f) : (Safe+Max)*0.5f;
        }
    };
}

UBTT_RangedCombat::UBTT_RangedCombat()
{
    NodeName = TEXT("Ranged Combat: Retreat / Fire / Approach");
    bCreateNodeInstance = true;
    bNotifyTick = true;
    bNotifyTaskFinished = true;
    BlackboardKey.SelectedKeyName = TEXT("AttackTarget");
    BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_RangedCombat, BlackboardKey), AActor::StaticClass());
}

ERangedCombatAction UBTT_RangedCombat::SelectAction(const AArcherEnemy& Enemy, float Distance, bool bHasLOS, ERangedCombatAction Previous)
{
    const FRanges Range(Enemy);
    if (!FMath::IsFinite(Distance)) return ERangedCombatAction::Approach;
    if (Distance < Range.Retreat || (Previous == ERangedCombatAction::Retreat && Distance < Range.Safe)) return ERangedCombatAction::Retreat;
    if (Distance > Range.Max || !bHasLOS || (Previous == ERangedCombatAction::Approach && Distance > Range.Approach)) return ERangedCombatAction::Approach;
    return ERangedCombatAction::Attack;
}

bool UBTT_RangedCombat::FindMoveGoal(AArcherEnemy& Enemy, AActor& Target, ERangedCombatAction Mode, FVector& OutGoal)
{
    UWorld* World = Enemy.GetWorld();
    auto* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
    if (!Nav || Mode == ERangedCombatAction::Attack) return false;
    const ANavigationData* Data = Nav->GetNavDataForProps(Enemy.GetNavAgentPropertiesRef(), Enemy.GetNavAgentLocation());
    if (!Data) return false;
    const FRanges Range(Enemy);
    const FVector Origin = Enemy.GetActorLocation();
    const FVector TargetLocation = Target.GetActorLocation();
    FVector Away = (Origin-TargetLocation).GetSafeNormal2D();
    if (Away.IsNearlyZero()) Away = -Enemy.GetActorForwardVector().GetSafeNormal2D();
    const float CurrentDistance = FVector::Dist2D(Origin,TargetLocation);
    // Try several distances: a narrow room may have no navigable point on the ideal ring.
    const float MinimumFiringDistance = Range.Retreat + 75.0f;
    const float PreferredDistance = FMath::Max(MinimumFiringDistance, Range.Approach - 100.0f);
    const TArray<float> Radii = Mode == ERangedCombatAction::Retreat
        ? TArray<float>{(Range.Safe + Range.Approach) * 0.5f, Range.Safe, CurrentDistance + 150.0f, CurrentDistance + 75.0f}
        : TArray<float>{PreferredDistance, (MinimumFiringDistance + PreferredDistance) * 0.5f, MinimumFiringDistance};
    float BestScore = -FLT_MAX;
    auto Consider = [&](const FVector& Desired, float Angle)
    {
        FNavLocation Projected;
        if (!Nav->ProjectPointToNavigation(Desired,Projected,FVector(100,100,250),Data)) return;
        const float CandidateDistance = FVector::Dist2D(Projected.Location,TargetLocation);
        if (CandidateDistance > Range.Max) return;
        if (Mode == ERangedCombatAction::Retreat && CandidateDistance < CurrentDistance+50.0f) return;
        if (Mode == ERangedCombatAction::Approach && CandidateDistance < MinimumFiringDistance) return;
        // A blocked firing lane must not repeatedly choose a point already within MoveTo's acceptance radius.
        if (FVector::DistSquared2D(Origin, Projected.Location) < FMath::Square(75.0f)) return;
        UNavigationPath* Path = UNavigationSystemV1::FindPathToLocationSynchronously(World,Enemy.GetNavAgentLocation(),Projected.Location,&Enemy);
        if (!Path || !Path->IsValid() || Path->IsPartial() || Path->PathPoints.Num()<2) return;
        if (Mode == ERangedCombatAction::Retreat)
        {
            bool bRunsPastTarget = false;
            const float MinimumClearance = FMath::Max(0.0f,FMath::Min(CurrentDistance-10.0f,200.0f));
            for (int32 i=1;i<Path->PathPoints.Num();++i)
            {
                FVector A=Path->PathPoints[i-1], B=Path->PathPoints[i], T=TargetLocation;
                A.Z=B.Z=T.Z=0;
                if (FMath::PointDistToSegment(T,A,B)<MinimumClearance) { bRunsPastTarget=true; break; }
            }
            if (bRunsPastTarget) return;
        }
        const FVector Eye = Projected.Location + FVector(0,0,Enemy.GetSimpleCollisionHalfHeight());
        const bool bClear = Enemy.HasClearRangedShotFrom(Eye, &Target);
        const float Progress = Mode == ERangedCombatAction::Retreat
            ? FMath::Min(CandidateDistance, Range.Safe)
            : FMath::Clamp(CurrentDistance - CandidateDistance, -200.0f, 500.0f);
        const float Score = (bClear ? 2000.0f : 0.0f) + Progress - Path->GetPathLength()*0.25f - FMath::Abs(Angle)*0.1f;
        if (Score>BestScore) { BestScore=Score; OutGoal=Projected.Location; }
    };
    for (const float Radius : Radii)
    {
        for (const float Angle : {0.0f,30.0f,-30.0f,60.0f,-60.0f,90.0f,-90.0f,135.0f,-135.0f,180.0f})
        {
            Consider(TargetLocation + Away.RotateAngleAxis(Angle,FVector::UpVector)*Radius, Angle);
        }
    }
    if (BestScore == -FLT_MAX && Mode == ERangedCombatAction::Retreat)
    {
        // Escape in smaller steps when a corner prevents reaching the safe distance in one move.
        for (const float Step : {200.0f, 100.0f})
        {
            for (const float Angle : {0.0f, 45.0f, -45.0f, 90.0f, -90.0f})
            {
                Consider(Origin + Away.RotateAngleAxis(Angle, FVector::UpVector) * Step, Angle);
            }
        }
    }
    return BestScore>-FLT_MAX;
}

EBTNodeResult::Type UBTT_RangedCombat::ExecuteTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory)
{
    RestoreFacing();
    Action=ERangedCombatAction::Attack;
    NextDecisionTime=NextRepathTime=0.0;
    LastTargetLocation=FVector(FLT_MAX);
    if (!Cast<AArcherEnemy>(OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr)) return EBTNodeResult::Failed;
    return EBTNodeResult::InProgress;
}

void UBTT_RangedCombat::TickTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory,float DeltaSeconds)
{
    AAIController* AI=OwnerComp.GetAIOwner();
    auto* Enemy=Cast<AArcherEnemy>(AI ? AI->GetPawn() : nullptr);
    UBlackboardComponent* BB=OwnerComp.GetBlackboardComponent();
    if (!Enemy || !BB || !UCombatFunctionLibrary::CanActorAttack(Enemy))
    {
        Cleanup(OwnerComp);
        if (!Enemy || !BB) FinishLatentTask(OwnerComp,EBTNodeResult::Failed);
        return;
    }
    const double Now=Enemy->GetWorld()->GetTimeSeconds();
    if (Now<NextDecisionTime) return;
    NextDecisionTime=Now+0.15;
    AActor* Target=Cast<AActor>(BB->GetValueAsObject(BlackboardKey.SelectedKeyName));
    if (!AArcherEnemy::IsLivingRangedTarget(Target) || Target==Enemy)
    {
        Target=Enemy->ResolveRangedTarget();
        if (Target) BB->SetValueAsObject(BlackboardKey.SelectedKeyName,Target);
        else BB->ClearValue(BlackboardKey.SelectedKeyName);
    }
    if (!Target)
    {
        Cleanup(OwnerComp);
        return;
    }
    const ERangedCombatAction Previous=Action;
    Action=SelectAction(*Enemy,FVector::Dist2D(Enemy->GetActorLocation(),Target->GetActorLocation()),Enemy->HasClearRangedShot(Target),Previous);
    if (Action==ERangedCombatAction::Attack)
    {
        if (AI->GetMoveStatus()!=EPathFollowingStatus::Idle) AI->StopMovement();
        SetMovementFacing(*Enemy, false);
        AI->SetFocus(Target);
        Enemy->TryStartRangedAttack(Target);
        return;
    }
    // Gameplay focus overrides path-following focus, causing a forward walk to slide backwards.
    AI->ClearFocus(EAIFocusPriority::Gameplay);
    SetMovementFacing(*Enemy, true);
    Enemy->StopAnimMontage(Enemy->AttackMontage);
    const float Speed=FMath::IsFinite(Enemy->RangedMovementSpeed) ? FMath::Max(1.0f,Enemy->RangedMovementSpeed) : 500.0f;
    Enemy->GetCharacterMovement()->MaxWalkSpeed=Speed;
    if (Now>=NextRepathTime || Action!=Previous)
    {
        NextRepathTime=Now+0.4;
        if (AI->GetMoveStatus()!=EPathFollowingStatus::Moving || Action!=Previous ||
            FVector::DistSquared2D(LastTargetLocation,Target->GetActorLocation())>FMath::Square(100.0f))
        {
            FVector Goal;
            if (FindMoveGoal(*Enemy,*Target,Action,Goal))
            {
                AI->MoveToLocation(Goal,40.0f,false,true,true,false,nullptr,false);
                LastTargetLocation=Target->GetActorLocation();
            }
            else AI->StopMovement();
        }
    }
}

void UBTT_RangedCombat::SetMovementFacing(AArcherEnemy& Enemy, bool bMoving)
{
    UCharacterMovementComponent* Movement = Enemy.GetCharacterMovement();
    if (FacingEnemy.Get() != &Enemy)
    {
        RestoreFacing();
        FacingEnemy = &Enemy;
        bSavedControllerYaw = Enemy.bUseControllerRotationYaw;
        bSavedOrientToMovement = Movement->bOrientRotationToMovement;
        bSavedControllerDesiredRotation = Movement->bUseControllerDesiredRotation;
    }
    // Turn along the current path while moving; use target focus only while standing to fire.
    Enemy.bUseControllerRotationYaw = !bMoving;
    Movement->bOrientRotationToMovement = bMoving;
    Movement->bUseControllerDesiredRotation = false;
}

void UBTT_RangedCombat::RestoreFacing()
{
    if (AArcherEnemy* Enemy = FacingEnemy.Get())
    {
        Enemy->bUseControllerRotationYaw = bSavedControllerYaw;
        Enemy->GetCharacterMovement()->bOrientRotationToMovement = bSavedOrientToMovement;
        Enemy->GetCharacterMovement()->bUseControllerDesiredRotation = bSavedControllerDesiredRotation;
    }
    FacingEnemy.Reset();
}

void UBTT_RangedCombat::Cleanup(UBehaviorTreeComponent& OwnerComp)
{
    if (AAIController* AI=OwnerComp.GetAIOwner())
    {
        AI->StopMovement();
        AI->ClearFocus(EAIFocusPriority::Gameplay);
        if (auto* Enemy=Cast<AArcherEnemy>(AI->GetPawn())) Enemy->StopAnimMontage(Enemy->AttackMontage);
    }
    RestoreFacing();
}

EBTNodeResult::Type UBTT_RangedCombat::AbortTask(UBehaviorTreeComponent& OwnerComp,uint8* NodeMemory)
{
    Cleanup(OwnerComp);
    return EBTNodeResult::Aborted;
}

void UBTT_RangedCombat::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    Cleanup(OwnerComp);
    Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
