// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/PlayerWeaponWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Subsystem/ViewModel/WeaponViewModel.h"

void UPlayerWeaponWidget::NativeConstruct()
{
    Super::NativeConstruct();

    BindViewModel();
}

void UPlayerWeaponWidget::NativeDestruct()
{
    UnbindViewModel();

    Super::NativeDestruct();
}

void UPlayerWeaponWidget::UpdatePlayerResourceBar(float CurrentResource, float MaxResource)
{
    float ResourcePercent = FMath::Clamp(CurrentResource / MaxResource, 0.0f, 1.0f);

    if (PlayerResourceBar)
    {
        PlayerResourceBar->SetPercent(ResourcePercent);
    }

    if (CurrentEnergyText && MaxEnergyText)
    {
        CurrentEnergyText->SetText(FText::AsNumber(FMath::FloorToInt(CurrentResource)));
        MaxEnergyText->SetText(FText::AsNumber(FMath::FloorToInt(MaxResource)));
    }
}

void UPlayerWeaponWidget::UpdateMainWeaponInfo(UWeaponDataAsset* InDataAsset)
{
    if(WeaponIconImage)
    {
        WeaponIconImage->SetBrushFromTexture(InDataAsset->WeaponIcon);
    }
    if(WeaponName)
    {
        WeaponName->SetText(InDataAsset->WeaponName);
    }
    if (UsageResourceText)
    {
        FText InWeaponCost = FText::AsNumber(FMath::FloorToInt(InDataAsset->AttackCost));
        UsageResourceText->SetText(InWeaponCost);
    }
}

void UPlayerWeaponWidget::UpdateSubWeaponInfo(UWeaponDataAsset* InDataAsset)
{
    if (SubWeaponIconImage)
    {
        SubWeaponIconImage->SetBrushFromTexture(InDataAsset->WeaponIcon);
    }
}

void UPlayerWeaponWidget::SetViewModel(UWeaponViewModel* InViewModel)
{
    UnbindViewModel();
    WeaponViewModel = InViewModel;
    BindViewModel();
}

void UPlayerWeaponWidget::BindViewModel()
{
    // 이미 바인딩된 경우 무시
    if (WeaponViewModel && !bIsViewModelBound)
    {
        // Model -> ViewModel 바인딩
        WeaponViewModel->OnPlayerResourceUpdate.AddDynamic(this, &UPlayerWeaponWidget::UpdatePlayerResourceBar);
        WeaponViewModel->OnMainWeaponUpdate.AddDynamic(this, &UPlayerWeaponWidget::UpdateMainWeaponInfo);
        WeaponViewModel->OnSubWeaponUpdate.AddDynamic(this, &UPlayerWeaponWidget::UpdateSubWeaponInfo);

        bIsViewModelBound = true;
    }
}

void UPlayerWeaponWidget::UnbindViewModel()
{
    if (WeaponViewModel && bIsViewModelBound)
    {
        WeaponViewModel->OnPlayerResourceUpdate.RemoveDynamic(this, &UPlayerWeaponWidget::UpdatePlayerResourceBar);
    
        bIsViewModelBound = false;
    }
}