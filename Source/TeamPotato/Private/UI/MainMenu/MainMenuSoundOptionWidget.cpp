// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/MainMenuSoundOptionWidget.h"
#include "Subsystem/AudioSubsystem.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UMainMenuSoundOptionWidget::NativeConstruct()
{
    Super::NativeConstruct();

    MusicSlider->OnValueChanged.AddDynamic(this, &UMainMenuSoundOptionWidget::UpdateMusicVolumeText);
    SFXSlider->OnValueChanged.AddDynamic(this, &UMainMenuSoundOptionWidget::UpdateSFXVolumeText);
    ApplyButton->OnClicked.AddDynamic(this, &UMainMenuSoundOptionWidget::OnApplyButtonClicke);

    if (CachedAudioSubsystem = GetGameInstance()->GetSubsystem<UAudioSubsystem>())
    {
        // --- 저장된 설정 불러오기 ---
        float SavedMusicVolume = CachedAudioSubsystem->GetMusicVolume();
        float SavedSFXVolume = CachedAudioSubsystem->GetSFXVolume();
        MusicSlider->SetValue(SavedMusicVolume);
        SFXSlider->SetValue(SavedSFXVolume);
        UpdateMusicVolumeText(SavedMusicVolume);
        UpdateSFXVolumeText(SavedSFXVolume);
    }
}

void UMainMenuSoundOptionWidget::UpdateMusicVolumeText(float InVolume)
{
    if (InVolume < 0) return;
    
    MusicSoundPercent->SetText(FText::AsPercent(InVolume));
}

void UMainMenuSoundOptionWidget::UpdateSFXVolumeText(float InVolume)
{
    if (InVolume < 0) return;

    SFXSoundPercent->SetText(FText::AsPercent(InVolume));
}

void UMainMenuSoundOptionWidget::OnApplyButtonClicke()
{
    if (CachedAudioSubsystem)
    {
        CachedAudioSubsystem->SetMusicVolume(MusicSlider->GetValue());
        CachedAudioSubsystem->SetSFXVolume(SFXSlider->GetValue());
    }
}
