// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveGameSubsystem.generated.h"

class USaveSettings;

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API USaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
    // --- 로컬에 설정 저장 ---
    bool SaveSettings(USaveSettings* Settings);
    // --- 다른 곳에서 이 서브시스템의 설정 불러오기 ---
    USaveSettings* GetGameSettings();

protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

public:
    bool bIsInitialized = false;

private:
    // --- 슬롯 이름 ---
    const FString SettingSaveSlotName = "SettingSaveSlot";


    // --- 설정 캐싱 ---
    USaveSettings* CachedSettings = nullptr;
};
