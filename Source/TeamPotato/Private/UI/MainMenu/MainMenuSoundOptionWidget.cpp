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
    ApplyButton->OnClicked.AddDynamic(this, &UMainMenuSoundOptionWidget::OnApplyButtonClick);
    CancelButton->OnClicked.AddDynamic(this, &UMainMenuSoundOptionWidget::OnCancelButtonClick);
    CloseButton->OnClicked.AddDynamic(this, &UMainMenuSoundOptionWidget::OnCloseButtonClick);

    if (CachedAudioSubsystem = GetGameInstance()->GetSubsystem<UAudioSubsystem>())
    {
        LoadSavedVolumeFromSubsystem();
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

void UMainMenuSoundOptionWidget::OnApplyButtonClick()
{
    if (!CachedAudioSubsystem)
    {
        CachedAudioSubsystem = GetGameInstance()->GetSubsystem<UAudioSubsystem>();
    }
    if (CachedAudioSubsystem)
    {
        CachedAudioSubsystem->SetMusicVolume(MusicSlider->GetValue(), this);
        CachedAudioSubsystem->SetSFXVolume(SFXSlider->GetValue(), this);
    }
}

void UMainMenuSoundOptionWidget::OnCancelButtonClick()
{
    LoadSavedVolumeFromSubsystem();
}

void UMainMenuSoundOptionWidget::OnCloseButtonClick()
{
    UE_LOG(LogTemp, Warning, TEXT("Close Button Clicked in Sound Option Widget"));
    OnCloseButtonClickedDelegate.Broadcast();
}


void UMainMenuSoundOptionWidget::LoadSavedVolumeFromSubsystem()
{
    if (!CachedAudioSubsystem) return;

    // --- 저장된 설정 불러오기 ---
    float SavedMusicVolume = CachedAudioSubsystem->GetMusicVolume();
    float SavedSFXVolume = CachedAudioSubsystem->GetSFXVolume();
    MusicSlider->SetValue(SavedMusicVolume);
    SFXSlider->SetValue(SavedSFXVolume);
    UpdateMusicVolumeText(SavedMusicVolume);
    UpdateSFXVolumeText(SavedSFXVolume);
}
