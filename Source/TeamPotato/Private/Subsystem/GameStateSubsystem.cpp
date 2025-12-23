// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/GameStateSubsystem.h"
#include "Common/MyGameSettings.h"
#include "Data/LevelDataAsset.h"
#include "Kismet/GameplayStatics.h"

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
	}
}

void UGameStateSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UGameStateSubsystem::SetGameState(EGameState NewState)
{
	if (CurrentGameState == NewState) return;

	// 새로운 상태로 변경
	EGameState OldState = CurrentGameState;
	CurrentGameState = NewState;

	OnGameStateChanged.Broadcast(OldState, NewState);
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

void UGameStateSubsystem::ExecuteLevelTravel(const FLevelData& LevelData)
{
	SetGameState(LevelData.GameState);

	if (!LevelData.LevelAsset.IsNull())
	{
		UE_LOG(LogTemp, Log, TEXT("Traveling to level: %s"), *LevelData.LevelAsset.ToString());

		FString LevelPath = LevelData.LevelAsset.ToString();
		
		// OpenLevel을 활용한 레벨 이동
		UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelPath));

		// LoadStreamLevel을 이용한 레벨 이동
	}
	else if (!LevelData.LevelName.IsNone())
	{
		UE_LOG(LogTemp, Log, TEXT("Traveling to level: %s"), *LevelData.LevelName.ToString());

		UGameplayStatics::OpenLevel(GetWorld(), LevelData.LevelName);
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelData for state %d has no valid level asset or name."), static_cast<int32>(LevelData.GameState));
	}
}
