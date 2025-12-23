// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/SaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem/Save/SaveSettings.h"
#include "Common/MyGameSettings.h"

void USaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // 초기화 시에 저장된 설정이 있는지 확인하고 로드
    if (UGameplayStatics::DoesSaveGameExist(SettingSaveSlotName, 0))
    {
        CachedSettings = 
            Cast<USaveSettings>(UGameplayStatics::LoadGameFromSlot(SettingSaveSlotName, 0));
    }
    // 만약 저장된 설정이 없다면 기본 설정 생성
    else
    {
        CachedSettings = 
            Cast<USaveSettings>(UGameplayStatics::CreateSaveGameObject(USaveSettings::StaticClass()));
    }

    // 다른 서브시스템과의 타이밍 조정
    // 저장된 정보를 받아야하는 다른 서브시스템들에게 알림
    bIsInitialized = true;
}

void USaveGameSubsystem::Deinitialize()
{
   
    Super::Deinitialize();
}

bool USaveGameSubsystem::SaveSettings()
{
    // 설정을 실제 슬롯에 저장
    if (UGameplayStatics::SaveGameToSlot(CachedSettings, SettingSaveSlotName, 0))
    {
        return true;
    }
    // 만약 저장에 실패하면 false 반환
    return false;
}

USaveSettings* USaveGameSubsystem::GetGameSettings()
{
    if (CachedSettings)
    {
        return CachedSettings;
    }
    // 캐시된 설정이 없으면 새로 생성
    return nullptr;
}

void USaveGameSubsystem::SaveMusicVolume(float InVolume)
{
    if (CachedSettings)
    {
        CachedSettings->MusicVolume = InVolume;
        SaveSettings();
    }
}

void USaveGameSubsystem::SaveSFXVolume(float InVolume)
{
    if (CachedSettings)
    {
        CachedSettings->SFXVolume = InVolume;
        SaveSettings();
    }
}
