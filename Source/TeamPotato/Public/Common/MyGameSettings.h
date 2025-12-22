// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Engine/DataTable.h"
#include "Common/GameStateTypes.h"
#include "MyGameSettings.generated.h"

class ULevelDataAsset;
class USoundMix;
/**
 * 
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Game Settings"))
class TEAMPOTATO_API UMyGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UMyGameSettings();

	UFUNCTION(BlueprintPure, Category = "Game Settings")
	static UMyGameSettings* Get();
	
	// 서브시스템에서 접근하기 위한 레벨 데이터 에셋
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Levels")
	TSoftObjectPtr<ULevelDataAsset> LevelDataAsset;

	// 시작 게임 상태
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Game State")
	EGameState DefaultStartState = EGameState::MainMenu;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Character")
	TSoftObjectPtr<UDataTable> CharacterDataTable;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Sound")
    TSoftObjectPtr<USoundMix> MasterSoundMix;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Sound")
    TSoftObjectPtr<USoundClass> MusicSoundClass;

    UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Sound")
    TSoftObjectPtr<USoundClass> SFXSoundClass;
};
