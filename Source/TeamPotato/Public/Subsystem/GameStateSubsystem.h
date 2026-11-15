// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Common/GameStateTypes.h"
#include "GameStateSubsystem.generated.h"

class ULevelDataAsset;
class UUserWidget;
struct FStreamableHandle;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameStateChanged, EGameState, OldState, EGameState, NewState);
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UGameStateSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// =============================================================================
	// 초기화, 종료
	// =============================================================================
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// =============================================================================
	// 게임 상태 관리
	// =============================================================================
	UFUNCTION(BlueprintPure, Category = "Game State")
	EGameState GetCurrentGameState() const { return CurrentGameState; }

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void SetGameState(EGameState NewState);

	// =============================================================================
	// 맵 이동
	// =============================================================================
	UFUNCTION(BlueprintCallable, Category = "Game State")
	void TravelToState(EGameState TargetState);

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void TravelToLobby();

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void TravelToMainMenu();

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void TravelToChapter(int32 ChapterIndex);

	void ShowLoadingScreen(UWorld* DungeonWorld);
	void FinishDungeonLoading(UWorld* DungeonWorld);

	// =============================================================================
	// Setter / Getter
	// =============================================================================
	ULevelDataAsset* GetLevelDdataAsset() const { return LevelDataAsset; }

	// =============================================================================
	// 델리게이트
	// =============================================================================
	FOnGameStateChanged OnGameStateChanged;

private:
	// 실제 레벨 이동 처리
	void ExecuteLevelTravel(const FLevelData& LevelData);
	void OpenLoadedLevel(FSoftObjectPath LevelPath, EGameState TargetState);
	void HandleMapLoaded(UWorld* World);
	void HandleTravelFailure(UWorld* World, ETravelFailure::Type Failure, const FString& Error);
	void AddLoadingOverlay(UWorld* World);
	void RevealAfterDraw(UWorld* World);
	void OnViewportDrawn();
	void RemoveLoadingOverlay();

private:
	UPROPERTY()
	EGameState CurrentGameState = EGameState::None;

	UPROPERTY()
	TObjectPtr<ULevelDataAsset> LevelDataAsset;

	UPROPERTY(Transient)
	TSubclassOf<UUserWidget> LoadingWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> ActiveLoadingScreen;

	TSharedPtr<FStreamableHandle> PendingMapLoad;
	bool bTravelInProgress = false;
	TWeakObjectPtr<UWorld> DungeonBeingGenerated;
	TWeakObjectPtr<UWorld> WorldReadyToDraw;
	uint64 ReadyFrame = 0;
};
