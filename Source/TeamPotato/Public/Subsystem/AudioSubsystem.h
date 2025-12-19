// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "AudioSubsystem.generated.h"

class USoundMix;
class USaveGameSubsystem;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UAudioSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // --- 게임 시작 시 딱 한번 저장된 설정을 불러와서 적용 ---
    void InitializeSaveSettings();

public:
    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetMusicVolume(float InVolume);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void SetSFXVolume(float InVolume);

    UFUNCTION(BlueprintCallable, Category = "Audio")
    float GetMusicVolume() const { return CurrnetMusicVolume; };

    UFUNCTION(BlueprintCallable, Category = "Audio")
    float GetSFXVolume() const { return CurrnetSFXVolume; };

private:
    // --- 사운드 믹스와 클래스 캐싱 ---
    USoundMix* CachedMasterSoundMix = nullptr;
    USoundClass* CachedMusicSoundClass = nullptr;
    USoundClass* CachedSFXSoundClass = nullptr;

    TObjectPtr<USaveGameSubsystem> CachedSaveGameSubsystem = nullptr;

    float CurrnetMusicVolume = 1.0f;
    float CurrnetSFXVolume = 1.0f;
};
