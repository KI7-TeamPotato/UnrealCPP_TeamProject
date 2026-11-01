#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "AI/Task/BTT_RangedCombat.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Combat/CombatAbilitySystemComponent.h"
#include "Combat/CombatAttributeSet.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Enemy/ArcherEnemy.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Player/TestCharacter.h"
#include <limits>

namespace
{
    struct FRangedEnemyWorld
    {
        UWorld* World;
        uint64 InitialFrame = GFrameCounter;
        FRangedEnemyWorld()
        {
            World = UWorld::CreateWorld(EWorldType::GamePreview, false);
            GEngine->CreateNewWorldContext(EWorldType::GamePreview).SetCurrentWorld(World);
            // GamePreview worlds omit AISystem by default; Blackboard initialization requires it.
            World->CreateAISystem();
            World->InitializeActorsForPlay(FURL());
        }
        ~FRangedEnemyWorld()
        {
            World->DestroyWorld(false);
            GEngine->DestroyWorldContext(World);
            GFrameCounter = InitialFrame;
        }
        template<class T> T* Spawn(FVector Position, UClass* Class = T::StaticClass())
        {
            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            return World->SpawnActor<T>(Class, Position, FRotator::ZeroRotator, Params);
        }
        UBlackboardComponent* Control(AArcherEnemy* Enemy)
        {
            if (AController* Existing = Enemy->GetController()) Existing->UnPossess();
            auto* AI = Spawn<AAIController>(FVector::ZeroVector);
            AI->Possess(Enemy);
            auto* Data = NewObject<UBlackboardData>(AI);
            FBlackboardEntry Entry;
            Entry.EntryName = TEXT("AttackTarget");
            Entry.KeyType = NewObject<UBlackboardKeyType_Object>(Data);
            Data->Keys.Add(Entry);
            UBlackboardComponent* Blackboard = nullptr;
            // UseBlackboard can return true even when InitializeBlackboard failed.
            return AI->UseBlackboard(Data, Blackboard) && Blackboard && Blackboard->GetBlackboardAsset() == Data
                ? Blackboard : nullptr;
        }
        AActor* Wall(FVector Position)
        {
            auto* Actor = Spawn<AActor>(Position);
            auto* Box = NewObject<UBoxComponent>(Actor);
            Actor->AddInstanceComponent(Box);
            Actor->SetRootComponent(Box);
            Box->InitBoxExtent(FVector(20, 90, 150));
            Box->SetCollisionProfileName(TEXT("BlockAll"));
            Box->RegisterComponent();
            Actor->SetActorLocation(Position);
            return Actor;
        }
        void Advance(float Seconds)
        {
            // Only advance time: level-dependent player/enemy BeginPlay and AI are deliberately excluded.
            for (float Time = 0; Time < Seconds; Time += 0.05f)
            {
                ++GFrameCounter;
                World->Tick(LEVELTICK_TimeOnly, 0.05f);
            }
        }
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRangedDistanceBandsTest, "TeamPotato.AI.Ranged.DistanceBandsAndHysteresis",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRangedDistanceBandsTest::RunTest(const FString& Parameters)
{
    FRangedEnemyWorld Fixture;
    auto* Enemy = Fixture.Spawn<AArcherEnemy>(FVector(0, 0, 100));
    using EAction = ERangedCombatAction;
    EAction State = UBTT_RangedCombat::SelectAction(*Enemy, 1500, true, EAction::Attack);
    TestEqual(TEXT("Far target starts approach"), State, EAction::Approach);
    State = UBTT_RangedCombat::SelectAction(*Enemy, 1000, true, State);
    TestEqual(TEXT("Approach continues past attack boundary until preferred distance"), State, EAction::Approach);
    State = UBTT_RangedCombat::SelectAction(*Enemy, 850, true, State);
    TestEqual(TEXT("Preferred distance switches to attack"), State, EAction::Attack);
    State = UBTT_RangedCombat::SelectAction(*Enemy, 440, true, State);
    TestEqual(TEXT("Close player immediately takes priority over firing"), State, EAction::Retreat);
    State = UBTT_RangedCombat::SelectAction(*Enemy, 600, true, State);
    TestEqual(TEXT("Retreat persists through the lower hysteresis band"), State, EAction::Retreat);
    State = UBTT_RangedCombat::SelectAction(*Enemy, 700, true, State);
    TestEqual(TEXT("Safe distance allows attack again"), State, EAction::Attack);
    TestEqual(TEXT("Lost firing lane requires repositioning"),
        UBTT_RangedCombat::SelectAction(*Enemy, 800, false, State), EAction::Approach);
    TestEqual(TEXT("Close distance still wins when line of sight is blocked"),
        UBTT_RangedCombat::SelectAction(*Enemy, 300, false, EAction::Approach), EAction::Retreat);
    TestTrue(TEXT("Invalid distance never authorizes firing"),
        UBTT_RangedCombat::SelectAction(*Enemy, std::numeric_limits<float>::quiet_NaN(), true, State) != EAction::Attack);

    auto* Other = Fixture.Spawn<AArcherEnemy>(FVector(0, 2000, 100));
    Other->RetreatStartDistance = 200;
    Other->RetreatStopDistance = 300;
    Other->ApproachStopDistance = 500;
    Other->MaxRangedAttackDistance = 700;
    TestEqual(TEXT("Second monster uses its own safe distance"),
        UBTT_RangedCombat::SelectAction(*Other, 600, true, EAction::Retreat), EAction::Attack);
    TestEqual(TEXT("First monster retains its independently configured hysteresis"),
        UBTT_RangedCombat::SelectAction(*Enemy, 600, true, EAction::Retreat), EAction::Retreat);
    TestTrue(TEXT("Runtime decision state is instantiated per monster"), GetDefault<UBTT_RangedCombat>()->HasInstance());

    FVector Goal(123, 456, 789);
    auto* Target = Fixture.Spawn<ATestCharacter>(FVector(300, 0, 100));
    TestFalse(TEXT("Retreat without navigation returns failure instead of an unvalidated destination"),
        UBTT_RangedCombat::FindMoveGoal(*Enemy, *Target, EAction::Retreat, Goal));
    TestFalse(TEXT("Approach without navigation does not invent a path through obstacles"),
        UBTT_RangedCombat::FindMoveGoal(*Enemy, *Target, EAction::Approach, Goal));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRangedTargetAndLOSTest, "TeamPotato.AI.Ranged.TargetReplacementAndLineOfFire",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRangedTargetAndLOSTest::RunTest(const FString& Parameters)
{
    FRangedEnemyWorld Fixture;
    auto* Enemy = Fixture.Spawn<AArcherEnemy>(FVector(0, 0, 100));
    auto* Player = Fixture.Spawn<ATestCharacter>(FVector(800, 0, 100));
    auto* PC = Fixture.Spawn<APlayerController>(FVector::ZeroVector);
    PC->Possess(Player);
    auto* Blackboard = Fixture.Control(Enemy);
    if (!TestNotNull(TEXT("Ranged controller has a target Blackboard"), Blackboard)) return false;
    Blackboard->SetValueAsObject(TEXT("AttackTarget"), Player);
    TestTrue(TEXT("Controller Blackboard stores the explicit AttackTarget"),
        Blackboard->GetValueAsObject(TEXT("AttackTarget")) == Player);
    TestTrue(TEXT("Blackboard target is resolved"), Enemy->ResolveRangedTarget() == Player);
    TestTrue(TEXT("Unobstructed firing lane is accepted"), Enemy->HasClearRangedShot(Player));

    AActor* Wall = Fixture.Wall(FVector(400, 0, 100));
    TestFalse(TEXT("Wall blocks firing"), Enemy->HasClearRangedShot(Player));
    Wall->SetActorLocation(FVector(50, 0, 100));
    TestFalse(TEXT("Muzzle offset cannot bypass a wall directly in front of the monster"), Enemy->HasClearRangedShot(Player));
    Wall->SetActorLocation(FVector(200, 0, 100));
    Wall->AttachToActor(Enemy, FAttachmentTransformRules::KeepWorldTransform);
    TestTrue(TEXT("Attached weapon does not block its owner's firing lane"), Enemy->HasClearRangedShot(Player));
    Wall->Destroy();

    auto* WidgetActor = Fixture.Spawn<AActor>(FVector(400, 0, 100));
    auto* Widget = NewObject<UWidgetComponent>(WidgetActor);
    WidgetActor->AddInstanceComponent(Widget);
    WidgetActor->SetRootComponent(Widget);
    Widget->SetWidgetSpace(EWidgetSpace::World);
    Widget->SetDrawSize(FVector2D(300, 300));
    Widget->UpdateBodySetup(true);
    Widget->SetCollisionProfileName(TEXT("BlockAll"));
    Widget->RegisterComponent();
    WidgetActor->SetActorLocation(FVector(400, 0, 100));
    FHitResult WidgetHit;
    FCollisionQueryParams WidgetTraceParams(SCENE_QUERY_STAT(RangedAITestWidget), false, Enemy);
    TestTrue(TEXT("Widget fixture actually blocks an unfiltered visibility trace"),
        Fixture.World->LineTraceSingleByChannel(WidgetHit, Enemy->GetActorLocation(), Player->GetActorLocation(),
            ECC_Visibility, WidgetTraceParams) && WidgetHit.GetComponent() == Widget);
    TestTrue(TEXT("World-space health bar is ignored by firing-lane selection"), Enemy->HasClearRangedShot(Player));
    WidgetActor->Destroy();

    auto* Alternate = Fixture.Spawn<ATestCharacter>(FVector(750, 300, 100));
    TestTrue(TEXT("Alternate target has initialized positive GAS health"),
        CastChecked<UCombatAbilitySystemComponent>(Alternate->GetAbilitySystemComponent())->GetHealth() > 0.0f);
    TestTrue(TEXT("Alternate target is a living ranged target"), AArcherEnemy::IsLivingRangedTarget(Alternate));
    Blackboard->SetValueAsObject(TEXT("AttackTarget"), Alternate);
    TestTrue(TEXT("Controller Blackboard accepts a replacement AttackTarget"),
        Blackboard->GetValueAsObject(TEXT("AttackTarget")) == Alternate);
    TestTrue(TEXT("Explicit live Blackboard target takes precedence over player zero"), Enemy->ResolveRangedTarget() == Alternate);
    auto* Replacement = Fixture.Spawn<ATestCharacter>(FVector(900, -200, 100));
    PC->Possess(Replacement);
    Blackboard->SetValueAsObject(TEXT("AttackTarget"), Player);
    Player->Destroy();
    TestTrue(TEXT("Destroyed character selection target is replaced by the newly possessed player"),
        Enemy->ResolveRangedTarget() == Replacement);
    auto* ASC = CastChecked<UCombatAbilitySystemComponent>(Replacement->GetAbilitySystemComponent());
    ASC->SetNumericAttributeBase(UCombatAttributeSet::GetHealthAttribute(), 0.0f);
    TestFalse(TEXT("Zero-health player is not a living ranged target"), AArcherEnemy::IsLivingRangedTarget(Replacement));
    TestNull(TEXT("No stale target remains after the current player dies"), Enemy->ResolveRangedTarget());
    TestFalse(TEXT("Null target is rejected"), AArcherEnemy::IsLivingRangedTarget(nullptr));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRangedAttackCadenceTest, "TeamPotato.AI.Ranged.AttackCadenceAndSafety",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRangedAttackCadenceTest::RunTest(const FString& Parameters)
{
    FRangedEnemyWorld Fixture;
    UClass* Class = LoadClass<AArcherEnemy>(nullptr, TEXT("/Game/Enemy/Blueprint/BP_Archer.BP_Archer_C"));
    if (!TestNotNull(TEXT("Real ranged monster Blueprint loads"), Class)) return false;
    auto* Enemy = Fixture.Spawn<AArcherEnemy>(FVector(0, 0, 100), Class);
    auto* Target = Fixture.Spawn<ATestCharacter>(FVector(800, 0, 100));
    Enemy->RangedAttackInterval = 3.0f;
    if (!TestNotNull(TEXT("Ranged monster animation instance initializes"), Enemy->GetMesh()->GetAnimInstance())) return false;
    if (!TestNotNull(TEXT("Ranged monster attack montage is configured"), Enemy->AttackMontage)) return false;
    Target->SetActorLocation(FVector(200, 0, 100));
    TestFalse(TEXT("Close target cannot start a ranged attack"), Enemy->TryStartRangedAttack(Target));
    Target->SetActorLocation(FVector(1500, 0, 100));
    TestFalse(TEXT("Distant target must be approached before firing"), Enemy->TryStartRangedAttack(Target));
    Target->SetActorLocation(FVector(800, 0, 100));
    AActor* Wall = Fixture.Wall(FVector(400, 0, 100));
    TestFalse(TEXT("Occluded target cannot start a ranged attack"), Enemy->TryStartRangedAttack(Target));
    Wall->Destroy();
    TestTrue(TEXT("Rejected distance and LOS attempts do not consume attack cooldown"), Enemy->TryStartRangedAttack(Target));
    TestTrue(TEXT("Successful attack starts the actual monster montage"), Enemy->GetCurrentMontage() == Enemy->AttackMontage);
    TestFalse(TEXT("Active montage cannot be restarted by another attack tick"), Enemy->TryStartRangedAttack(Target));
    Enemy->StopAnimMontage(Enemy->AttackMontage);
    TestFalse(TEXT("Cancelling montage does not bypass the attack interval"), Enemy->TryStartRangedAttack(Target));
    auto* OtherEnemy = Fixture.Spawn<AArcherEnemy>(FVector(0, 2000, 100), Class);
    auto* OtherTarget = Fixture.Spawn<ATestCharacter>(FVector(800, 2000, 100));
    TestTrue(TEXT("One monster's cooldown does not suppress another monster"), OtherEnemy->TryStartRangedAttack(OtherTarget));
    OtherEnemy->StopAnimMontage(OtherEnemy->AttackMontage);
    const float Cooldown = FMath::Max(Enemy->RangedAttackInterval, Enemy->AttackMontage->GetPlayLength() + 0.05f) + 0.1f;
    Fixture.Advance(Cooldown);
    TestTrue(TEXT("Attack becomes available after its interval"), Enemy->TryStartRangedAttack(Target));
    Enemy->StopAnimMontage(Enemy->AttackMontage);
    Fixture.Advance(Cooldown);
    CastChecked<UCombatAbilitySystemComponent>(Target->GetAbilitySystemComponent())
        ->SetNumericAttributeBase(UCombatAttributeSet::GetHealthAttribute(), 0.0f);
    TestFalse(TEXT("Dead player cannot restart a ranged attack"), Enemy->TryStartRangedAttack(Target));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRangedTreeIntegrationTest, "TeamPotato.AI.Ranged.RealBehaviorTreeIntegration",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRangedTreeIntegrationTest::RunTest(const FString& Parameters)
{
    struct FTreeCase { const TCHAR* Tree; const TCHAR* Blackboard; };
    for (const FTreeCase& Entry : {
        FTreeCase{TEXT("/Game/Enemy/AI/Normal/Base_Range/BT_RangeEnemyBase.BT_RangeEnemyBase"),
            TEXT("/Game/Enemy/AI/Normal/Base_Range/BB_RangeEnemyBase.BB_RangeEnemyBase")},
        FTreeCase{TEXT("/Game/Enemy/AI/Elite/Magic/BT_Magic.BT_Magic"),
            TEXT("/Game/Enemy/AI/Elite/Magic/BB_Magic.BB_Magic")}})
    {
        auto* Tree = LoadObject<UBehaviorTree>(nullptr, Entry.Tree);
        auto* Blackboard = LoadObject<UBlackboardData>(nullptr, Entry.Blackboard);
        if (!TestNotNull(TEXT("Ranged behavior tree asset loads"), Tree) ||
            !TestNotNull(TEXT("Matching ranged Blackboard asset loads"), Blackboard)) continue;
        TestTrue(TEXT("Behavior tree uses its own matching Blackboard"), Tree->BlackboardAsset == Blackboard);
        TestTrue(TEXT("AttackTarget is a valid Blackboard key"), Blackboard->GetKeyID(TEXT("AttackTarget")) != FBlackboard::InvalidKey);
        const auto TargetType = Blackboard->GetKeyType(Blackboard->GetKeyID(TEXT("AttackTarget")));
        TestTrue(TEXT("AttackTarget accepts actor objects"), TargetType && TargetType->IsChildOf(UBlackboardKeyType_Object::StaticClass()));
        if (!TestNotNull(TEXT("Ranged tree has a root selector"), Tree->RootNode.Get())) continue;
        if (!TestTrue(TEXT("Ranged tree has an active combat branch"), !Tree->RootNode->Children.IsEmpty())) continue;
        auto* Task = Cast<UBTT_RangedCombat>(Tree->RootNode->Children[0].ChildTask.Get());
        if (TestNotNull(TEXT("Highest priority branch runs the unified ranged combat task"), Task))
        {
            TestEqual(TEXT("Combat task reads AttackTarget"), Task->GetSelectedBlackboardKey(), FName(TEXT("AttackTarget")));
            TestTrue(TEXT("Saved task instantiates state for every monster"), Task->HasInstance());
        }
    }
    return true;
}

#endif
