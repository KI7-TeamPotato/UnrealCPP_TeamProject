#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "AI/Task/BTT_RangedCombat.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Composites/BTComposite_Selector.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Enemy/ArcherEnemy.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationData.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NavMesh/NavMeshBoundsVolume.h"

namespace
{
    struct FRangedNavigationWorld
    {
        UWorld* World = nullptr;
        UNavigationSystemV1* Nav = nullptr;
        const uint64 InitialFrame = GFrameCounter;

        FRangedNavigationWorld()
        {
            // NavigationObjectRepository is a UWorldSubsystem, which is excluded from GamePreview worlds.
            // It must exist when UNavigationSystemV1 is constructed so the floor can enter the nav octree.
            World = UWorld::CreateWorld(EWorldType::Game, false);
            GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
            World->CreateAISystem();
            World->InitializeActorsForPlay(FURL());
        }

        ~FRangedNavigationWorld()
        {
            World->DestroyWorld(false);
            GEngine->DestroyWorldContext(World);
            GFrameCounter = InitialFrame;
        }

        template<class T> T* Spawn(const FVector& Location)
        {
            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            return World->SpawnActor<T>(Location, FRotator::ZeroRotator, Params);
        }

        bool BuildNavigation(bool bNarrow = false)
        {
            TGuardValue<TEnumAsByte<EWorldType::Type>> EditorBuild(World->WorldType, EWorldType::Editor);
            auto* Floor = Spawn<AStaticMeshActor>(FVector(0, 0, -50));
            auto* Mesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
            if (!Floor || !Mesh) return false;
            Floor->GetStaticMeshComponent()->SetStaticMesh(Mesh);
            Floor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
            Floor->SetActorScale3D(bNarrow ? FVector(9, 5.5, 1) : FVector(60, 60, 1));

            // A box supplies volume bounds without a saved level or an UnrealEd brush-builder dependency.
            auto* Volume = Spawn<ANavMeshBoundsVolume>(FVector::ZeroVector);
            auto* Bounds = NewObject<UBoxComponent>(Volume);
            Volume->AddInstanceComponent(Bounds);
            Volume->SetRootComponent(Bounds);
            Bounds->InitBoxExtent(FVector(2800, 2800, 400));
            Bounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            Bounds->SetCanEverAffectNavigation(false);
            Bounds->RegisterComponent();
            Nav = NewObject<UNavigationSystemV1>(World);
            World->SetNavigationSystem(Nav);
            Nav->InitializeForWorld(*World, FNavigationSystemRunMode::EditorMode);
            Nav->Build();
            ANavigationData* Data = Nav->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
            if (!Data) return false;
            Data->EnsureBuildCompletion();
            FNavLocation Projected;
            return Nav->ProjectPointToNavigation(FVector::ZeroVector, Projected, FVector(100, 100, 250));
        }

        void Advance(AArcherEnemy& Enemy, AAIController& AI, float Seconds)
        {
            auto* Brain = CastChecked<UBehaviorTreeComponent>(AI.GetBrainComponent());
            auto* Following = AI.GetPathFollowingComponent();
            auto* Movement = Enemy.GetCharacterMovement();
            for (float Time = 0; Time < Seconds; Time += 0.05f)
            {
                ++GFrameCounter;
                // Keep unrelated enemy/player BeginPlay out of this isolated fixture while running the real AI components.
                World->Tick(LEVELTICK_TimeOnly, 0.05f);
                Brain->TickComponent(0.05f, LEVELTICK_All, nullptr);
                Following->TickComponent(0.05f, LEVELTICK_All, nullptr);
                Movement->TickComponent(0.05f, LEVELTICK_All, nullptr);
            }
        }
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRangedNavigationFallbackTest, "TeamPotato.AI.Ranged.NavMeshFallbackGoals",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRangedNavigationFallbackTest::RunTest(const FString& Parameters)
{
    {
        FRangedNavigationWorld Fixture;
        if (!TestTrue(TEXT("Narrow-room NavMesh builds"), Fixture.BuildNavigation(true))) return false;
        auto* Enemy = Fixture.Spawn<AArcherEnemy>(FVector(0, 0, 96));
        auto* Target = Fixture.Spawn<ACharacter>(FVector(200, 0, 96));
        Enemy->GetCapsuleComponent()->SetCanEverAffectNavigation(false);
        Target->GetCapsuleComponent()->SetCanEverAffectNavigation(false);
        FVector Goal;
        if (TestTrue(TEXT("Room without preferred retreat ring still offers a closer fallback"),
            UBTT_RangedCombat::FindMoveGoal(*Enemy, *Target, ERangedCombatAction::Retreat, Goal)))
        {
            TestTrue(TEXT("Fallback increases separation inside the small room"), FVector::Dist2D(Goal, Target->GetActorLocation()) > 250);
            auto* Path = UNavigationSystemV1::FindPathToLocationSynchronously(Fixture.World, Enemy->GetNavAgentLocation(), Goal, Enemy);
            TestTrue(TEXT("Narrow-room fallback has a complete path"), Path && Path->IsValid() && !Path->IsPartial());
        }
    }
    {
        FRangedNavigationWorld Fixture;
        if (!TestTrue(TEXT("Occlusion fixture NavMesh builds"), Fixture.BuildNavigation())) return false;
        auto* Enemy = Fixture.Spawn<AArcherEnemy>(FVector(0, 0, 96));
        auto* Target = Fixture.Spawn<ACharacter>(FVector(800, 0, 96));
        auto* Pillar = Fixture.Spawn<AActor>(FVector(400, 0, 100));
        auto* Box = NewObject<UBoxComponent>(Pillar);
        Pillar->AddInstanceComponent(Box);
        Pillar->SetRootComponent(Box);
        Box->InitBoxExtent(FVector(40, 160, 200));
        Box->SetCollisionProfileName(TEXT("BlockAll"));
        Box->SetCanEverAffectNavigation(false);
        Box->RegisterComponent();
        Pillar->SetActorLocation(FVector(400, 0, 100));
        TestFalse(TEXT("Pillar actually blocks the current firing lane"), Enemy->HasClearRangedShot(Target));
        FVector Goal;
        if (TestTrue(TEXT("Occluded target gets a real repositioning destination"),
            UBTT_RangedCombat::FindMoveGoal(*Enemy, *Target, ERangedCombatAction::Approach, Goal)))
        {
            TestTrue(TEXT("Repositioning goal lies outside MoveTo's 40-unit acceptance radius"),
                FVector::Dist2D(Enemy->GetActorLocation(), Goal) > 40);
            auto* Path = UNavigationSystemV1::FindPathToLocationSynchronously(Fixture.World, Enemy->GetNavAgentLocation(), Goal, Enemy);
            TestTrue(TEXT("Pillar repositioning has a complete path"), Path && Path->IsValid() && !Path->IsPartial());
        }
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRangedNavigationMovementTest, "TeamPotato.AI.Ranged.NavMeshApproachAndRetreat",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRangedNavigationMovementTest::RunTest(const FString& Parameters)
{
    FRangedNavigationWorld Fixture;
    if (!TestTrue(TEXT("Fixture builds a queryable Recast NavMesh"), Fixture.BuildNavigation())) return false;
    auto* Enemy = Fixture.Spawn<AArcherEnemy>(FVector(0, 0, 96));
    auto* Target = Fixture.Spawn<ACharacter>(FVector(250, 0, 96));
    if (!TestNotNull(TEXT("Ranged enemy spawns"), Enemy) || !TestNotNull(TEXT("Target spawns"), Target)) return false;
    Target->GetCapsuleComponent()->SetCanEverAffectNavigation(false);
    Target->SetActorEnableCollision(false);
    Enemy->GetCapsuleComponent()->SetCanEverAffectNavigation(false);
    auto* AI = Fixture.Spawn<AAIController>(FVector::ZeroVector);
    if (!TestNotNull(TEXT("AI controller spawns"), AI)) return false;
    AI->Possess(Enemy);
    Enemy->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

    for (const ERangedCombatAction Mode : { ERangedCombatAction::Retreat, ERangedCombatAction::Approach })
    {
        Target->SetActorLocation(Mode == ERangedCombatAction::Retreat ? FVector(250, 0, 96) : FVector(1700, 0, 96));
        FVector Goal;
        if (!TestTrue(TEXT("FindMoveGoal selects a reachable NavMesh location"), UBTT_RangedCombat::FindMoveGoal(*Enemy, *Target, Mode, Goal))) return false;
        auto* Path = UNavigationSystemV1::FindPathToLocationSynchronously(Fixture.World, Enemy->GetNavAgentLocation(), Goal, Enemy);
        if (!TestNotNull(TEXT("Selected location produces a real navigation path"), Path)) return false;
        TestTrue(TEXT("Selected path is valid and complete"), Path->IsValid() && !Path->IsPartial() && Path->PathPoints.Num() >= 2);
        const float Before = FVector::Dist2D(Enemy->GetActorLocation(), Target->GetActorLocation());
        const float After = FVector::Dist2D(Goal, Target->GetActorLocation());
        TestTrue(TEXT("Goal changes distance in the requested direction"), Mode == ERangedCombatAction::Retreat ? After > Before + 50 : After < Before - 50);
    }

    auto* Blackboard = NewObject<UBlackboardData>(AI);
    FBlackboardEntry Key;
    Key.EntryName = TEXT("AttackTarget");
    auto* KeyType = NewObject<UBlackboardKeyType_Object>(Blackboard);
    KeyType->BaseClass = AActor::StaticClass();
    Key.KeyType = KeyType;
    Blackboard->Keys.Add(Key);
    auto* Tree = NewObject<UBehaviorTree>(AI);
    Tree->BlackboardAsset = Blackboard;
    Tree->RootNode = NewObject<UBTComposite_Selector>(Tree);
    auto* Task = NewObject<UBTT_RangedCombat>(Tree);
    Tree->RootNode->Children.AddDefaulted_GetRef().ChildTask = Task;
    Tree->RootNode->InitializeNode(nullptr, 0, 0, 0);
    Task->InitializeNode(Tree->RootNode, 1, 0, 1);
    Tree->RootNode->InitializeComposite(1);
    if (!TestTrue(TEXT("AI starts the actual ranged combat behavior tree"), AI->RunBehaviorTree(Tree))) return false;

    Target->SetActorLocation(FVector(250, 0, 96));
    AI->GetBlackboardComponent()->SetValueAsObject(TEXT("AttackTarget"), Target);
    Fixture.Advance(*Enemy, *AI, 0.25f);
    TestTrue(TEXT("Ranged BT task issues a path-following movement request"), AI->GetMoveStatus() == EPathFollowingStatus::Moving);
    Fixture.Advance(*Enemy, *AI, 2.0f);
    const float RetreatDistance = FVector::Dist2D(Enemy->GetActorLocation(), Target->GetActorLocation());
    TestTrue(TEXT("Character movement physically retreats to the safe band"), RetreatDistance >= Enemy->RetreatStopDistance - 50);
    TestTrue(TEXT("Retreat does not leave maximum firing range"), RetreatDistance <= Enemy->MaxRangedAttackDistance);

    AI->StopMovement();
    Enemy->SetActorLocation(FVector(0, 0, 96), false, nullptr, ETeleportType::TeleportPhysics);
    Enemy->GetCharacterMovement()->StopMovementImmediately();
    Target->SetActorLocation(FVector(1700, 0, 96));
    Fixture.Advance(*Enemy, *AI, 3.0f);
    const float ApproachDistance = FVector::Dist2D(Enemy->GetActorLocation(), Target->GetActorLocation());
    TestTrue(TEXT("Character movement physically approaches into the firing band"), ApproachDistance <= Enemy->ApproachStopDistance + 50);
    TestTrue(TEXT("Approach does not run into the player's retreat radius"), ApproachDistance >= Enemy->RetreatStartDistance);
    TestEqual(TEXT("Movement stops once the target is within the firing band"), AI->GetMoveStatus(), EPathFollowingStatus::Idle);
    CastChecked<UBehaviorTreeComponent>(AI->GetBrainComponent())->StopTree(EBTStopMode::Forced);
    return true;
}

#endif
