#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Component/WaveComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SceneComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "Engine/Engine.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "TimerManager.h"

namespace
{
    struct FWaveSpawnWorld
    {
        UWorld* World;
        AActor* Room;
        UWaveComponent* Wave;
        UArrowComponent* SpawnPoint;
        TArray<AActor*> Spawned;
        TArray<UNiagaraComponent*> WarningSystems;
        int32 SystemsPerWarning = 1;
        FDelegateHandle SpawnHandle;
        uint64 InitialFrame = GFrameCounter;

        FWaveSpawnWorld()
        {
            World = UWorld::CreateWorld(EWorldType::GamePreview, false);
            GEngine->CreateNewWorldContext(EWorldType::GamePreview).SetCurrentWorld(World);
            World->InitializeActorsForPlay(FURL());
            Room = World->SpawnActor<AActor>();
            auto* Root = NewObject<USceneComponent>(Room);
            Room->SetRootComponent(Root);
            Root->RegisterComponent();
            SpawnPoint = NewObject<UArrowComponent>(Room);
            SpawnPoint->SetupAttachment(Root);
            SpawnPoint->RegisterComponent();
            SpawnPoint->SetWorldLocation(FVector(400, 500, 100));
            Wave = NewObject<UWaveComponent>(Room);
            Room->AddInstanceComponent(Wave);
            Wave->RegisterComponent();
            // Only the room begins play. Enemy BeginPlay requires gameplay/UI subsystems.
            Room->DispatchBeginPlay();
            FWaveData Data;
            FSpawnInfo Entry;
            Entry.EnemyClass = AEnemyCharacter::StaticClass();
            Entry.SpawnPointIndex = 0;
            Data.SpawnList.Add(Entry);
            Wave->Waves.Add(Data);
            SpawnHandle = World->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateLambda(
                [this](AActor* Actor)
                {
                    if (Actor->GetOwner() != Room) return;
                    Spawned.Add(Actor);
                    if (!Actor->IsA<AStaticMeshActor>()) return;
                    for (int32 Index = 0; Index < SystemsPerWarning; ++Index)
                    {
                        auto* System = NewObject<UNiagaraComponent>(Actor);
                        Actor->AddInstanceComponent(System);
                        System->SetupAttachment(Actor->GetRootComponent());
                        System->SetAutoActivate(false);
                        System->SetAsset(NewObject<UNiagaraSystem>(System));
                        System->RegisterComponent();
                        // Headless tests drive the actual completion delegate without Niagara simulation.
                        System->SetActiveFlag(true);
                        WarningSystems.Add(System);
                    }
                }));
            // Prime this frame so timers scheduled by StartWaveSystem become active immediately.
            World->GetTimerManager().Tick(0.0f);
        }

        ~FWaveSpawnWorld()
        {
            World->RemoveOnActorSpawnedHandler(SpawnHandle);
            World->DestroyWorld(false);
            GEngine->DestroyWorldContext(World);
            GFrameCounter = InitialFrame;
        }

        void Start() { Wave->StartWaveSystem({SpawnPoint}); }

        void Finish(UNiagaraComponent* System)
        {
            System->SetActiveFlag(false);
            System->OnSystemFinished.Broadcast(System);
        }

        void Advance(float Seconds)
        {
            ++GFrameCounter;
            World->GetTimerManager().Tick(Seconds);
        }
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWaveSeparateEffectsTest, "TeamPotato.Wave.SpawnSequence.WaitForCompletion",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWaveSeparateEffectsTest::RunTest(const FString& Parameters)
{
    FWaveSpawnWorld Fixture;
    Fixture.SystemsPerWarning = 2;
    Fixture.Wave->EnemyWarningEffect = AStaticMeshActor::StaticClass();
    Fixture.Wave->EnemyEffect = AActor::StaticClass();
    // Invalid entries must not produce false warnings or affect live-enemy counts.
    FSpawnInfo MissingClass;
    Fixture.Wave->Waves[0].SpawnList.Add(MissingClass);
    FSpawnInfo MissingPoint;
    MissingPoint.EnemyClass = AEnemyCharacter::StaticClass();
    MissingPoint.SpawnPointIndex = 99;
    Fixture.Wave->Waves[0].SpawnList.Add(MissingPoint);
    Fixture.Start();
    if (!TestEqual(TEXT("Only the warning effect spawns immediately"), Fixture.Spawned.Num(), 1)) return false;
    AActor* Warning = Fixture.Spawned[0];
    TestTrue(TEXT("The dedicated warning class is used"), Warning->IsA<AStaticMeshActor>());
    TestEqual(TEXT("Warning is not counted as a living enemy"), Fixture.Wave->RemainingEnemies, 0);
    if (!TestEqual(TEXT("Both warning systems are present"), Fixture.WarningSystems.Num(), 2)) return false;
    TestTrue(TEXT("Niagara completion is observed"), Fixture.WarningSystems[0]->OnSystemFinished.IsBound());
    Fixture.Advance(2.0f);
    TestEqual(TEXT("Elapsed time alone cannot complete the warning"), Fixture.Spawned.Num(), 1);
    Fixture.Finish(Fixture.WarningSystems[1]);
    Fixture.Advance(0.06f);
    TestEqual(TEXT("Finishing one system still waits for the other"), Fixture.Spawned.Num(), 1);
    Fixture.Finish(Fixture.WarningSystems[0]);
    TestEqual(TEXT("Spawning is deferred until the completion broadcast returns"), Fixture.Spawned.Num(), 1);
    Fixture.Advance(0.06f);
    if (!TestEqual(TEXT("One enemy and one arrival effect follow the warning"), Fixture.Spawned.Num(), 3)) return false;
    TestTrue(TEXT("Enemy spawns before the arrival effect"), Fixture.Spawned[1]->IsA<AEnemyCharacter>());
    TestTrue(TEXT("Arrival uses the existing EnemyEffect class"), Fixture.Spawned[2]->GetClass() == AActor::StaticClass());
    TestTrue(TEXT("Warning is cleaned up when the enemy appears"), !IsValid(Warning) || Warning->IsActorBeingDestroyed());
    TestEqual(TEXT("Only successfully spawned enemies are counted"), Fixture.Wave->RemainingEnemies, 1);
    Fixture.Advance(2.0f);
    TestEqual(TEXT("Completed warnings do not produce duplicate actors"), Fixture.Spawned.Num(), 3);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWaveMultipleWarningsTest, "TeamPotato.Wave.SpawnSequence.MultipleWarnings",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWaveMultipleWarningsTest::RunTest(const FString& Parameters)
{
    FWaveSpawnWorld Fixture;
    Fixture.Wave->EnemyWarningEffect = AStaticMeshActor::StaticClass();
    Fixture.Wave->EnemyEffect = AActor::StaticClass();
    const FSpawnInfo SecondEnemy = Fixture.Wave->Waves[0].SpawnList[0];
    Fixture.Wave->Waves[0].SpawnList.Add(SecondEnemy);
    Fixture.Start();
    if (!TestEqual(TEXT("Each enemy location receives a warning"), Fixture.WarningSystems.Num(), 2)) return false;
    Fixture.Finish(Fixture.WarningSystems[0]);
    Fixture.Advance(1.5f);
    TestEqual(TEXT("The wave waits until every warning finishes"), Fixture.Spawned.Num(), 2);
    Fixture.Finish(Fixture.WarningSystems[1]);
    Fixture.Advance(0.06f);
    if (!TestEqual(TEXT("Both enemies and arrival effects spawn after both warnings"), Fixture.Spawned.Num(), 6)) return false;
    TestTrue(TEXT("Each arrival effect follows its enemy"),
        Fixture.Spawned[2]->IsA<AEnemyCharacter>() && Fixture.Spawned[3]->GetClass() == AActor::StaticClass() &&
        Fixture.Spawned[4]->IsA<AEnemyCharacter>() && Fixture.Spawned[5]->GetClass() == AActor::StaticClass());
    TestEqual(TEXT("Both enemies are counted"), Fixture.Wave->RemainingEnemies, 2);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWaveFallbackRestartTest, "TeamPotato.Wave.SpawnSequence.FallbackAndRestart",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWaveFallbackRestartTest::RunTest(const FString& Parameters)
{
    FWaveSpawnWorld Fixture;
    Fixture.Wave->EnemyEffect = AStaticMeshActor::StaticClass();
    Fixture.Start();
    if (!TestEqual(TEXT("Existing rooms get a warning without setting the new property"), Fixture.Spawned.Num(), 1)) return false;
    AActor* OldWarning = Fixture.Spawned[0];
    UNiagaraComponent* OldSystem = Fixture.WarningSystems[0];
    TestTrue(TEXT("Unconfigured warning falls back to EnemyEffect"), OldWarning->IsA<AStaticMeshActor>());
    Fixture.Advance(0.6f);
    Fixture.Start();
    TestTrue(TEXT("Restart removes the earlier warning"), !IsValid(OldWarning) || OldWarning->IsActorBeingDestroyed());
    if (!TestEqual(TEXT("Restart creates one replacement warning"), Fixture.Spawned.Num(), 2)) return false;
    TestFalse(TEXT("Restart unbinds the old warning completion"), OldSystem->OnSystemFinished.IsBound());
    Fixture.Finish(OldSystem);
    Fixture.Advance(2.0f);
    TestEqual(TEXT("The old warning cannot complete the restarted wave"), Fixture.Spawned.Num(), 2);
    Fixture.Finish(Fixture.WarningSystems[1]);
    Fixture.Advance(0.06f);
    if (!TestEqual(TEXT("Restarted wave waits for its own warning completion"), Fixture.Spawned.Num(), 4)) return false;
    TestTrue(TEXT("Restart still spawns enemy before its arrival effect"),
        Fixture.Spawned[2]->IsA<AEnemyCharacter>() && Fixture.Spawned[3]->IsA<AStaticMeshActor>());
    TestEqual(TEXT("Restart does not double count enemies"), Fixture.Wave->RemainingEnemies, 1);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWaveEndPlayCancellationTest, "TeamPotato.Wave.SpawnSequence.EndPlayCancellation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWaveEndPlayCancellationTest::RunTest(const FString& Parameters)
{
    FWaveSpawnWorld Fixture;
    Fixture.Wave->EnemyWarningEffect = AStaticMeshActor::StaticClass();
    Fixture.Wave->EnemyEffect = AActor::StaticClass();
    Fixture.Start();
    if (!TestEqual(TEXT("Warning is active before the room ends play"), Fixture.Spawned.Num(), 1)) return false;
    AActor* Warning = Fixture.Spawned[0];
    UNiagaraComponent* System = Fixture.WarningSystems[0];
    Fixture.Advance(0.5f);
    Fixture.Room->Destroy();
    TestTrue(TEXT("Ending the room removes its pending warning"), !IsValid(Warning) || Warning->IsActorBeingDestroyed());
    TestFalse(TEXT("Ending the room unbinds warning completion"), System->OnSystemFinished.IsBound());
    Fixture.Finish(System);
    Fixture.Advance(2.0f);
    TestEqual(TEXT("A destroyed room cannot spawn delayed enemies or effects"), Fixture.Spawned.Num(), 1);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWaveDestroyedWarningTest, "TeamPotato.Wave.SpawnSequence.DestroyedWarnings",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWaveDestroyedWarningTest::RunTest(const FString& Parameters)
{
    for (bool bDestroyActor : {false, true})
    {
        FWaveSpawnWorld Fixture;
        Fixture.Wave->EnemyWarningEffect = AStaticMeshActor::StaticClass();
        Fixture.Wave->EnemyEffect = AActor::StaticClass();
        Fixture.Start();
        if (!TestEqual(TEXT("Warning exists before destruction"), Fixture.WarningSystems.Num(), 1)) return false;
        if (bDestroyActor) Fixture.Spawned[0]->Destroy();
        else Fixture.WarningSystems[0]->DestroyComponent();
        Fixture.Advance(0.06f);
        TestEqual(TEXT("A destroyed warning cannot stall the wave"), Fixture.Spawned.Num(), 3);
        TestEqual(TEXT("Destroyed-warning recovery spawns one enemy"), Fixture.Wave->RemainingEnemies, 1);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWaveMissingWarningTest, "TeamPotato.Wave.SpawnSequence.NoPlayableWarning",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWaveMissingWarningTest::RunTest(const FString& Parameters)
{
    for (bool bAssignWarningActor : {false, true})
    {
        FWaveSpawnWorld Fixture;
        Fixture.SystemsPerWarning = 0;
        if (bAssignWarningActor)
        {
            Fixture.Wave->EnemyWarningEffect = AStaticMeshActor::StaticClass();
            Fixture.Wave->EnemyEffect = AActor::StaticClass();
        }
        Fixture.Start();
        Fixture.Advance(0.06f);
        TestEqual(TEXT("Missing Niagara does not impose a one-second delay or stall the wave"), Fixture.Wave->RemainingEnemies, 1);
        TestEqual(TEXT("Only the configured effects and enemy are created"), Fixture.Spawned.Num(), bAssignWarningActor ? 3 : 1);
    }
    return true;
}

#endif
