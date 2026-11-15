#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Combat/CombatFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Enemy/BossBase.h"
#include "Engine/Engine.h"
#include "Engine/Level.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Player/TestCharacter.h"
#include "Sound/SoundBase.h"
#include "TimerManager.h"
#include "TeamPotato/Logic/DungeonGanarator.h"
#include "TeamPotato/Room/RoomBase.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDungeonBossFloorsTest, "TeamPotato.Dungeon.FixedBossFloors",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FDungeonBossFloorsTest::RunTest(const FString& Parameters)
{
    UWorld* Map = LoadObject<UWorld>(nullptr, TEXT("/Game/Maps/SpawnDunGeon.SpawnDunGeon"));
    if (!TestNotNull(TEXT("Gameplay map loads"), Map)) return false;
    ADungeonGanarator* Placed = nullptr;
    for (AActor* Actor : Map->PersistentLevel->Actors)
    {
        Placed = Cast<ADungeonGanarator>(Actor);
        if (Placed) break;
    }
    if (!TestNotNull(TEXT("Map has its configured generator"), Placed)) return false;

    UWorld* World = UWorld::CreateWorld(EWorldType::GamePreview, false);
    GEngine->CreateNewWorldContext(EWorldType::GamePreview).SetCurrentWorld(World);
    const uint64 SavedFrame = GFrameCounter;
    ON_SCOPE_EXIT
    {
        World->DestroyWorld(false);
        GEngine->DestroyWorldContext(World);
        GFrameCounter = SavedFrame;
    };
    World->InitializeActorsForPlay(FURL());
    auto* Dungeon = World->SpawnActor<ADungeonGanarator>(Placed->GetClass());
    if (!TestNotNull(TEXT("Generator spawns"), Dungeon)) return false;
    Dungeon->StageConfigMap = Placed->StageConfigMap;
    // Temporary tracks on this instance let the test run before the designer assigns music.
    const bool bTestAudio = GEngine->UseSound() && World->GetAudioDeviceRaw();
    if (bTestAudio)
    {
        World->bAllowAudioPlayback = true;
        Dungeon->DungeonMusic = LoadObject<USoundBase>(nullptr, TEXT("/Engine/EditorSounds/Notifications/CompileSuccess_Cue.CompileSuccess_Cue"));
        Dungeon->BossMusic = LoadObject<USoundBase>(nullptr, TEXT("/Engine/EditorSounds/Notifications/CompileFailed_Cue.CompileFailed_Cue"));
        if (!TestNotNull(TEXT("Exploration test sound"), Dungeon->DungeonMusic.Get()) ||
            !TestNotNull(TEXT("Encounter test sound"), Dungeon->BossMusic.Get())) return false;
    }
    auto CheckMusic = [&](const FString& Label, USoundBase* Expected)
    {
        if (!bTestAudio) return;
        TInlineComponentArray<UAudioComponent*> Components(Dungeon);
        int32 ActiveTracks = 0;
        for (UAudioComponent* Component : Components)
        {
            if (!Component->OnAudioFinishedNative.IsBoundToObject(Dungeon)) continue;
            ++ActiveTracks;
            TestEqual(Label, Component->Sound.Get(), Expected);
            TestTrue(Label + TEXT(" playing"), Component->IsPlaying());
        }
        TestEqual(Label + TEXT(" single current player"), ActiveTracks, 1);
    };
    // Exercise the normal completion and reset paths with one connecting room per floor.
    for (auto& Pair : Dungeon->StageConfigMap) Pair.Value.StageRoomAmount = 1;
    Dungeon->DispatchBeginPlay();
    auto Advance = [&]()
    {
        ++GFrameCounter;
        World->GetTimerManager().Tick(0.025f);
    };

    for (int32 Theme = 1; Theme <= 2; ++Theme)
    {
        const auto* ThemeConfig = Placed->StageConfigMap.Find(Theme);
        if (!TestTrue(TEXT("Chapter has a themed portal room"), ThemeConfig && !ThemeConfig->PotalRooms.IsEmpty())) return false;
        for (int32 Floor = 1; Floor <= 3; ++Floor)
        {
            const FString Label = FString::Printf(TEXT("Chapter %d stage %d"), Theme, Floor);
            TestEqual(Label + TEXT(" chapter"), Dungeon->Stage, Theme);
            TestEqual(Label + TEXT(" stage"), Dungeon->GetChapter(), Floor);
            ARoomBase* Room = nullptr;
            for (int32 Frame = 0; !Room && Frame < 500; ++Frame)
            {
                Advance();
                for (TActorIterator<ARoomBase> It(World); It; ++It)
                    if (It->GetOwner() == Dungeon && !It->IsActorBeingDestroyed()) { Room = *It; break; }
            }
            if (!TestNotNull(Label + TEXT(" generated end room"), Room)) return false;
            CheckMusic(Label + TEXT(" exploration before encounter"), Dungeon->DungeonMusic);
            if (Floor == 1)
            {
                TestEqual(Label + TEXT(" themed portal"), Room->GetClass(), ThemeConfig->PotalRooms[0].Get());
            }
            else
            {
                TestEqual(Label + TEXT(" fixed arena"), Room->GetClass()->GetName(), FString(Floor == 2
                    ? TEXT("BP_S1BossRoom_C") : TEXT("BP_DunGeonRoomS2_Boss_C")));
                UWaveComponent* Wave = Room->WaveSystemComp;
                if (!TestNotNull(Label + TEXT(" wave"), Wave)) return false;
                // Cosmetic warning completion is covered by the wave sequence tests.
                Wave->EnemyWarningEffect = nullptr;
                Wave->EnemyEffect = nullptr;
                if (!Room->HasActorBegunPlay()) Room->DispatchBeginPlay();
                auto* Trigger = Cast<UBoxComponent>(Room->GetDefaultSubobjectByName(TEXT("OnEnterRoomCollision")));
                if (!TestNotNull(Label + TEXT(" battle trigger"), Trigger)) return false;
                auto* Player = World->SpawnActor<ATestCharacter>(FVector(0, 0, 10000), FRotator::ZeroRotator);
                Trigger->OnComponentBeginOverlap.Broadcast(Trigger, Player, nullptr, 0, false, FHitResult());
                CheckMusic(Label + TEXT(" boss music on entry"), Dungeon->BossMusic);
                Advance();
                Advance();
                ABossBase* Boss = nullptr;
                for (TActorIterator<ABossBase> It(World); It; ++It)
                    if (It->EncounterRoom.Get() == Room) { Boss = *It; break; }
                if (!TestNotNull(Label + TEXT(" spawned boss"), Boss)) return false;
                TestEqual(Label + TEXT(" boss class"), Boss->GetClass()->GetName(),
                    FString(Floor == 2 ? TEXT("BP_ScareCrow_C") : TEXT("BP_LastBoss_C")));
                UCombatFunctionLibrary::ApplyCombatDamage(Boss, Boss->GetCombatHealth() + 1.0f, nullptr);
                TestEqual(Label + TEXT(" boss defeated"), Boss->GetCombatHealth(), 0.0f);
                ++GFrameCounter;
                World->GetTimerManager().Tick(3.1f);
                CheckMusic(Label + TEXT(" exploration after wave clear"), Dungeon->DungeonMusic);
                if (Player) Player->Destroy();
                int32 Exits = 0;
                for (TActorIterator<AActor> It(World, Dungeon->ChapterExitPortalClass); It; ++It)
                    if (!It->IsActorBeingDestroyed() && (It->GetOwner() == Room || It->GetOwner() == Dungeon)) ++Exits;
                TestEqual(Label + TEXT(" exit after boss death"), Exits, Floor == 2 || Theme == 1 ? 1 : 0);
            }
            if (Theme == 2 && Floor == 3) break;
            int32 CurrentFloor = Dungeon->GetChapter();
            Dungeon->ProcessEvent(Dungeon->FindFunctionChecked(TEXT("GoToNextStage")), &CurrentFloor);
        }
    }
    return true;
}
#endif
