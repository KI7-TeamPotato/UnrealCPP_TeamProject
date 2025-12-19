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

void UPlayerWeaponWidget::UpdatePlayerResourceBar(float NewResourcePercent)
{
    if (PlayerResourceBar)
    {
        PlayerResourceBar->SetPercent(NewResourcePercent);
    }
}

void UPlayerWeaponWidget::UpdateMainWeaponInfo(FText InWeaponName, UTexture2D* InWeaponIcon)
{
    if(WeaponIconImage)
    {
        WeaponIconImage->SetBrushFromTexture(InWeaponIcon);
    }
    if(WeaponName)
    {
        WeaponName->SetText(InWeaponName);
    }
}

void UPlayerWeaponWidget::UpdateSubWeaponInfo(UTexture2D* InSubWeaponIcon)
{
    if (SubWeaponIconImage)
    {
        SubWeaponIconImage->SetBrushFromTexture(InSubWeaponIcon);
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