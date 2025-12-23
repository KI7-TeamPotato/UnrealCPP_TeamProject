// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/GameStateSubsystem.h"
#include "Common/MyGameSettings.h"
#include "Data/LevelDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/AssetManager.h"
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

        if (!Settings->LoadingWidgetClass.IsNull())
        {
            // 로딩 화면 위젯 로드
            LoadingWidget = Settings->LoadingWidgetClass.LoadSynchronous();
        }
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


// TSoftObjectPtr<UWorld> => 에셋 경로와 타입을 나타내는 약한 참조
// FSoftObjectPath => 에셋 경로를 문자열로 나타냄
void UGameStateSubsystem::ExecuteLevelTravel(const FLevelData& LevelData)
{
    UUserWidget* LoadingScreen = nullptr;

    // 로딩 화면 표시
    if (LoadingWidget)
    {
        LoadingScreen = CreateWidget<UUserWidget>(GetWorld(), LoadingWidget.Get());
        if (LoadingScreen)
        {
            LoadingScreen->AddToViewport();
        }
    }
    
    // 게임 상태 변경
	SetGameState(LevelData.GameState);

	if (!LevelData.LevelAsset.IsNull())
	{
		//UE_LOG(LogTemp, Log, TEXT("Traveling to level: %s"), *LevelData.LevelAsset.GetAssetName());
        UE_LOG(LogTemp, Log, TEXT("Level asset path: %s"), *LevelData.LevelAsset.ToSoftObjectPath().ToString());

		FString LevelPath = LevelData.LevelAsset.GetAssetName();

		// OpenLevel과 비동기 맵 에셋 로드 이용
        TSoftObjectPtr<UWorld> LevelToLoad = LevelData.LevelAsset;
        // 비동기 에셋 로드 매니저(에셋 로드 요청, 완료 후 콜백)
        FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
        // 비동기 로드 요청 (로드할 레벨 경로, 로드 완료 후의 콜백) (현재 레벨 유지하다가 로드 완료 후 전환)
        Streamable.RequestAsyncLoad(LevelToLoad.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([this, LevelPath, LoadingScreen]()
            {
                // 로드 완료했다면 로딩 화면 제거
                if (LoadingScreen)
                {
                    LoadingScreen->RemoveFromParent();
                }

                // 레벨 전환
                UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelPath));
            }));
	}
	else if (!LevelData.LevelName.IsNone())
	{
        UE_LOG(LogTemp, Log, TEXT("Level name path: %s"), *LevelData.LevelName.ToString());

		UGameplayStatics::OpenLevel(GetWorld(), LevelData.LevelName);
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelData for state %d has no valid level asset or name."), static_cast<int32>(LevelData.GameState));
	}

    // OpenLevel을 활용한 레벨 이동
    //UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelPath));
}