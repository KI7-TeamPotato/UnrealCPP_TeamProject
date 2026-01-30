// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameMenu/PlayerStatPanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Subsystem/ViewModel/PlayerStatusViewModel.h"
#include "Subsystem/ViewModel/WeaponViewModel.h"
#include "Subsystem/CharacterSubsystem.h"
#include "Data/WeaponDataAsset.h"
#include "Kismet/GameplayStatics.h"

void UPlayerStatPanelWidget::NativeConstruct()
{
    Super::NativeConstruct();

}

void UPlayerStatPanelWidget::NativeDestruct()
{
    UnbindViewModel();

    Super::NativeDestruct();
}

void UPlayerStatPanelWidget::SetViewModel(UPlayerStatusViewModel* InPlayerViewModel, UWeaponViewModel* InWeaponViewModel)
{
    UnbindViewModel();

    PlayerStatusViewModel = InPlayerViewModel;
    PlayerWeaponViewModel = InWeaponViewModel;

    BindViewModel();
}

void UPlayerStatPanelWidget::BindViewModel()
{
    if (PlayerStatusViewModel && !bIsBound
        && PlayerWeaponViewModel && !bIsBound)
    {
        UE_LOG(LogTemp, Warning, TEXT("UPlayerStatPanelWidget::BindViewModel - Binding to PlayerStatusViewModel"));

        PlayerStatusViewModel->OnPlayerHealthChangedCurrentMaxHealth.AddDynamic(this, &UPlayerStatPanelWidget::UpdateHealthUI);
        PlayerStatusViewModel->OnPlayerWalkSpeedChanged.AddDynamic(this, &UPlayerStatPanelWidget::UpdateWalkSpeedUI);

        UE_LOG(LogTemp, Warning, TEXT("UPlayerStatPanelWidget::BindViewModel - Binding to WeaponViewModel"));

        PlayerWeaponViewModel->OnPlayerResourceUpdate.AddDynamic(this, &UPlayerStatPanelWidget::UpdateEnergyUI);
        PlayerWeaponViewModel->OnMainWeaponUpdate.AddDynamic(this, &UPlayerStatPanelWidget::UpdateWeaponDataUI);

        bIsBound = true;
    }
}

void UPlayerStatPanelWidget::UnbindViewModel()
{
    if(PlayerStatusViewModel && bIsBound)
    {
        PlayerStatusViewModel->OnPlayerHealthChangedCurrentMaxHealth.RemoveDynamic(this, &UPlayerStatPanelWidget::UpdateHealthUI);
        bIsBound = false;
    }
    if(PlayerWeaponViewModel && bIsBound)
    {
        PlayerWeaponViewModel->OnPlayerResourceUpdate.RemoveDynamic(this, &UPlayerStatPanelWidget::UpdateEnergyUI);
        bIsBound = false;
    }
}

//void UPlayerStatPanelWidget::OnPlayerStatHealthChanged(float InCurrentHealth, float InMaxHealth)
//{
//    UpdateHealthUI(InCurrentHealth, InMaxHealth);
//}
//
//void UPlayerStatPanelWidget::OnPlayerStatEnergyChanged(float CurrentResource, float MaxResource)
//{
//    UpdateEnergyUI(CurrentResource, MaxResource);
//}

void UPlayerStatPanelWidget::UpdateHealthUI(float InCurrentHealth, float InMaxHealth)
{
    if(HealthProgressBar)
    {
        float HealthPercent = FMath::Clamp(InCurrentHealth / InMaxHealth, 0.0f, 1.0f);
        HealthProgressBar->SetPercent(HealthPercent);
    }
    if(CurrentHealthText)
    {
        FText HealthText = FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), InCurrentHealth, InMaxHealth));
        CurrentHealthText->SetText(HealthText);
    }
    if(HealthStatText)
    {
        FText HealthStat = FText::FromString(FString::Printf(TEXT("%.0f"), InMaxHealth));
        HealthStatText->SetText(HealthStat);
    }
}

void UPlayerStatPanelWidget::UpdateEnergyUI(float CurrentResource, float MaxResource)
{
    if(EnergyProgressBar)
    {
        float EnergyPercent = FMath::Clamp(CurrentResource / MaxResource, 0.0f, 1.0f);
        EnergyProgressBar->SetPercent(EnergyPercent);
    }
    if(CurrentEnergyText)
    {
        FText EnergyText = FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), CurrentResource, MaxResource));
        CurrentEnergyText->SetText(EnergyText);
    }
    if(EnergyStatText)
    {
        FText EnergyStat = FText::FromString(FString::Printf(TEXT("%.0f"), MaxResource));
        EnergyStatText->SetText(EnergyStat);
    }
}

void UPlayerStatPanelWidget::UpdateWeaponDataUI(UWeaponDataAsset* InWeaponData)
{
    if (AttackDamageText)
    {
        if(InWeaponData)
        {
            FText AttackDamageStat = FText::FromString(FString::Printf(TEXT("%.0f"), InWeaponData->AttackDamage));
            AttackDamageText->SetText(AttackDamageStat);
        }
    }
}

void UPlayerStatPanelWidget::UpdateWalkSpeedUI(float NewWalkSpeed)
{
    if (WalkSpeedText)
    {
        FText WalkSpeedStat = FText::FromString(FString::Printf(TEXT("%.0f"), NewWalkSpeed));
        WalkSpeedText->SetText(WalkSpeedStat);
    }
}
