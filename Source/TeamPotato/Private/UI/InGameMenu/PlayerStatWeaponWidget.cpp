// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGameMenu/PlayerStatWeaponWidget.h"
#include "Components/Image.h"
#include "Subsystem/ViewModel/WeaponViewModel.h"

void UPlayerStatWeaponWidget::NativeConstruct()
{
    Super::NativeConstruct();

    BindViewModel();
}

void UPlayerStatWeaponWidget::NativeDestruct()
{
    UnbindViewModel();

    Super::NativeDestruct();
}

void UPlayerStatWeaponWidget::UpdateMainWeaponIcon(FText InWeaponName, UTexture2D* InWeaponIcon)
{
    if (InWeaponIcon)
    {
        MainWeaponIcon->SetBrushFromTexture(InWeaponIcon);
    }
}

void UPlayerStatWeaponWidget::UpdateSubWeaponIcon(UTexture2D* InSubWeaponIcon)
{
    if (InSubWeaponIcon)
    {
        SubWeaponIcon->SetBrushFromTexture(InSubWeaponIcon);
    }
}

void UPlayerStatWeaponWidget::SetViewModel(UWeaponViewModel* InViewModel)
{
    UnbindViewModel();
    WeaponViewModel = InViewModel;
    BindViewModel();
}

void UPlayerStatWeaponWidget::BindViewModel()
{
    // 이미 바인딩된 경우 무시
    if (WeaponViewModel && !bIsBound)
    {
        // Model -> ViewModel 바인딩
        WeaponViewModel->OnMainWeaponUpdate.AddDynamic(this, &UPlayerStatWeaponWidget::UpdateMainWeaponIcon);
        WeaponViewModel->OnSubWeaponUpdate.AddDynamic(this, &UPlayerStatWeaponWidget::UpdateSubWeaponIcon);

        bIsBound = true;
    }
}
void UPlayerStatWeaponWidget::UnbindViewModel()
{
    if (WeaponViewModel && bIsBound)
    {
        WeaponViewModel->OnMainWeaponUpdate.RemoveDynamic(this, &UPlayerStatWeaponWidget::UpdateMainWeaponIcon);
        WeaponViewModel->OnSubWeaponUpdate.RemoveDynamic(this, &UPlayerStatWeaponWidget::UpdateSubWeaponIcon);

        bIsBound = false;
    }
}