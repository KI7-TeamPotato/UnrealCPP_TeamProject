// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/AudioSubsystem.h"
#include "Common/MyGameSettings.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem/SaveGameSubsystem.h"
#include "Subsystem/Save/SaveSettings.h"


void UAudioSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // 다른 서브시스템에 대한 의존성 초기화, 게임 저장 서브시스템이 먼저 초기화되도록 보장
    Collection.InitializeDependency<USaveGameSubsystem>();

    // 사운드 믹스와 클래스 캐싱 로직 구현
    if (UMyGameSettings* GameSettings = UMyGameSettings::Get())
    {
        if (GameSettings->MasterSoundMix.IsValid())
        {
            CachedMasterSoundMix = GameSettings->MasterSoundMix.LoadSynchronous();;
        }

        if (GameSettings->MusicSoundClass.IsValid())
        {
            CachedMusicSoundClass = GameSettings->MusicSoundClass.LoadSynchronous();
        }

        if (GameSettings->SFXSoundClass.IsValid())
        {
            CachedSFXSoundClass = GameSettings->SFXSoundClass.LoadSynchronous();;
        }
    }

    // 초기화를 보장된 상태  
    if (CachedSaveGameSubsystem = GetGameInstance()->GetSubsystem<USaveGameSubsystem>())
    {
        // 이미 초기화가 완료된 상태라면 바로 설정 적용(한번 더 확인)
        if (CachedSaveGameSubsystem->bIsInitialized)
        {
            InitializeSaveSettings();
        }
    }
}

void UAudioSubsystem::InitializeSaveSettings()
{
    if (CachedSaveGameSubsystem)
    {
        SetMusicVolume(CachedSaveGameSubsystem->GetGameSettings()->MusicVolume);
        SetSFXVolume(CachedSaveGameSubsystem->GetGameSettings()->SFXVolume);
    }
}

void UAudioSubsystem::SetMusicVolume(float InVolume)
{
    CurrnetMusicVolume = InVolume;

    UGameplayStatics::SetSoundMixClassOverride(
        GetWorld(),
        CachedMasterSoundMix,
        CachedMusicSoundClass,
        InVolume
    );

    if (CachedSaveGameSubsystem)
    {
        CachedSaveGameSubsystem->SaveMusicVolume(InVolume);
    }
}

void UAudioSubsystem::SetSFXVolume(float InVolume)
{
    CurrnetSFXVolume = InVolume;

    UGameplayStatics::SetSoundMixClassOverride(
        GetWorld(),
        CachedMasterSoundMix,
        CachedSFXSoundClass,
        InVolume
    );

    if (CachedSaveGameSubsystem)
    {
        CachedSaveGameSubsystem->SaveSFXVolume(InVolume);
    }
}