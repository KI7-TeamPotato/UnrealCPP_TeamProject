// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/GameStateSubsystem.h"
#include "Common/MyGameSettings.h"
#include "Data/LevelDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/AssetManager.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "Blueprint/GameViewportSubsystem.h"
#include "Blueprint/UserWidget.h"

void UGameStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (const UMyGameSettings* Settings = UMyGameSettings::Get())
	{
		if (!Settings->LevelDataAsset.IsNull())
		{
			// 레벨 데이터 에셋 로드
			LevelDataAsset = Settings->LevelDataAsset.LoadSynchronous();

			UE_LOG(LogTemp, Log, TEXT("GameStateSubsystem: LevelDataAsset loaded successfully."));
		}
		SetGameState(Settings->DefaultStartState);

		LoadingWidgetClass = Settings->LoadingWidgetClass.LoadSynchronous();
	}

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandleMapLoaded);
	GEngine->OnTravelFailure().AddUObject(this, &ThisClass::HandleTravelFailure);
}

void UGameStateSubsystem::Deinitialize()
{
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	GEngine->OnTravelFailure().RemoveAll(this);
	if (PendingMapLoad)
	{
		PendingMapLoad->CancelHandle();
		PendingMapLoad.Reset();
	}
	RemoveLoadingOverlay();
	Super::Deinitialize();
}

void UGameStateSubsystem::SetGameState(EGameState NewState)
{
	if (CurrentGameState == NewState) return;

	// 새로운 상태로 변경
	EGameState OldState = CurrentGameState;
	CurrentGameState = NewState;

    UE_LOG(LogTemp, Log, TEXT("Game state changed from %d to %d"), static_cast<int32>(OldState), static_cast<int32>(NewState));

	OnGameStateChanged.Broadcast(OldState, NewState);
}

void UGameStateSubsystem::TravelToLobby()
{
	TravelToState(EGameState::Lobby);
}

void UGameStateSubsystem::TravelToMainMenu()
{
	TravelToState(EGameState::MainMenu);
}

void UGameStateSubsystem::TravelToChapter(int32 ChapterIndex)
{
	EGameState TargetState = EGameState::None;

    UE_LOG(LogTemp, Log, TEXT("TravelToChapter called with ChapterIndex: %d"), ChapterIndex);

	switch (ChapterIndex)
	{
	case 0:
		TargetState = EGameState::Chapter1;
		break;
	case 1:
		TargetState = EGameState::Chapter2;
		break;
	case 2:
		TargetState = EGameState::Chapter3;
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid chapter index: %d"), ChapterIndex);
		return;
	}

	TravelToState(TargetState);
}

void UGameStateSubsystem::TravelToState(EGameState TargetState)
{
    if (!LevelDataAsset) return;

    FLevelData LevelData;
    if (LevelDataAsset->GetLevelDataByState(TargetState, LevelData))
    {
        ExecuteLevelTravel(LevelData);
    }
}


void UGameStateSubsystem::ExecuteLevelTravel(const FLevelData& LevelData)
{
	UWorld* World = GetWorld();
	if (!World || bTravelInProgress || LevelData.LevelAsset.IsNull())
	{
		return;
	}

	bTravelInProgress = true;
	AddLoadingOverlay(World);
	const FSoftObjectPath LevelPath = LevelData.LevelAsset.ToSoftObjectPath();
	PendingMapLoad = UAssetManager::GetStreamableManager().RequestAsyncLoad(LevelPath,
		FStreamableDelegate::CreateUObject(this, &ThisClass::OpenLoadedLevel, LevelPath, LevelData.GameState));
	if (!PendingMapLoad)
	{
		bTravelInProgress = false;
		RemoveLoadingOverlay();
	}
}

void UGameStateSubsystem::OpenLoadedLevel(FSoftObjectPath LevelPath, EGameState TargetState)
{
	if (!PendingMapLoad)
	{
		return;
	}
	const bool bLoaded = IsValid(PendingMapLoad->GetLoadedAsset());
	PendingMapLoad.Reset();
	if (!bLoaded)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not load destination map %s"), *LevelPath.ToString());
		bTravelInProgress = false;
		RemoveLoadingOverlay();
		return;
	}
	SetGameState(TargetState);
	UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelPath.GetLongPackageName()));
}

void UGameStateSubsystem::ShowLoadingScreen(UWorld* DungeonWorld)
{
	if (DungeonWorld && DungeonWorld->GetGameInstance() == GetGameInstance())
	{
		DungeonBeingGenerated = DungeonWorld;
		AddLoadingOverlay(DungeonWorld);
	}
}

void UGameStateSubsystem::AddLoadingOverlay(UWorld* World)
{
	WorldReadyToDraw.Reset();
	if (ActiveLoadingScreen || !LoadingWidgetClass || !World->GetGameViewport())
	{
		return;
	}
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetGameInstance(), LoadingWidgetClass);
	FGameViewportWidgetSlot Slot;
	Slot.ZOrder = 1000;
	Slot.bAutoRemoveOnWorldRemoved = false;
	if (Widget && UGameViewportSubsystem::Get(World)->AddWidget(Widget, Slot))
	{
		ActiveLoadingScreen = Widget;
		World->GetGameViewport()->OnDrawn().AddUObject(this, &ThisClass::OnViewportDrawn);
	}
}

void UGameStateSubsystem::HandleMapLoaded(UWorld* World)
{
	if (!World || World->GetGameInstance() != GetGameInstance())
	{
		return;
	}
	bTravelInProgress = false;
	if (PendingMapLoad)
	{
		PendingMapLoad->CancelHandle();
		PendingMapLoad.Reset();
	}
	if (ActiveLoadingScreen)
	{
		ActiveLoadingScreen->SetPlayerContext(FLocalPlayerContext(GetGameInstance()->GetFirstGamePlayer(), World));
		if (DungeonBeingGenerated.Get() != World)
		{
			RevealAfterDraw(World);
		}
	}
}

void UGameStateSubsystem::FinishDungeonLoading(UWorld* DungeonWorld)
{
	if (DungeonWorld && DungeonBeingGenerated.Get() == DungeonWorld)
	{
		DungeonBeingGenerated.Reset();
		if (!bTravelInProgress)
		{
			RevealAfterDraw(DungeonWorld);
		}
	}
}

void UGameStateSubsystem::RevealAfterDraw(UWorld* World)
{
	WorldReadyToDraw = World;
	ReadyFrame = GFrameCounter;
}

void UGameStateSubsystem::OnViewportDrawn()
{
	UWorld* World = WorldReadyToDraw.Get();
	// LoadMap issues an unpresented draw in the same frame as PostLoadMap.
	if (World && World == GetWorld() && GFrameCounter > ReadyFrame &&
		World->GetGameViewport() && !World->GetGameViewport()->bDisableWorldRendering &&
		!World->ViewLocationsRenderedLastFrame.IsEmpty())
	{
		RemoveLoadingOverlay();
	}
}

void UGameStateSubsystem::HandleTravelFailure(UWorld* World, ETravelFailure::Type Failure, const FString& Error)
{
	if (World && World->GetGameInstance() == GetGameInstance() && bTravelInProgress)
	{
		UE_LOG(LogTemp, Warning, TEXT("Level travel failed: %s"), *Error);
		bTravelInProgress = false;
		RemoveLoadingOverlay();
	}
}

void UGameStateSubsystem::RemoveLoadingOverlay()
{
	WorldReadyToDraw.Reset();
	if (UGameViewportClient* Viewport = GetGameInstance()->GetGameViewportClient())
	{
		Viewport->OnDrawn().RemoveAll(this);
	}
	if (ActiveLoadingScreen)
	{
		ActiveLoadingScreen->RemoveFromParent();
		ActiveLoadingScreen = nullptr;
	}
}
