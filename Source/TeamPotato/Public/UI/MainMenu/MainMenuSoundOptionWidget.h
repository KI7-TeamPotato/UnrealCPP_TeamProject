// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuSoundOptionWidget.generated.h"

class USlider;
class UTextBlock;
class UButton;
class UAudioSubsystem;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UMainMenuSoundOptionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void LoadSavedVolumeFromSubsystem();

    UFUNCTION()
    void UpdateMusicVolumeText(float InVolume);

    UFUNCTION()
    void UpdateSFXVolumeText(float InVolume);

    UFUNCTION()
    void OnApplyButtonClick();

    UFUNCTION()
    void OnCancelButtonClick();

protected:
    // ---  배경 음악 볼륨 슬라이더 ---
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<USlider> MusicSlider;

    // ---  효과음 볼륨 슬라이더 ---
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<USlider> SFXSlider;

    // ---  배경 음악 볼륨을 텍스트로 표시 ---
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> MusicSoundPercent;

    // ---  효과음 볼륨을 텍스트로 표시 ---
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> SFXSoundPercent;

    // ---  적용 버튼 ---
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UButton> ApplyButton;

    // --- 취소 버튼 ---
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UButton> CancelButton;

    // --- 창 닫기 버튼 ---
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UButton> CloseButton;

private:
    TObjectPtr<UAudioSubsystem> CachedAudioSubsystem = nullptr;
};
