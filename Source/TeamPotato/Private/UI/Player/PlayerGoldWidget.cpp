// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/PlayerGoldWidget.h"
#include "Components/TextBlock.h"
#include "Subsystem/ViewModel/ItemViewModel.h"

void UPlayerGoldWidget::NativeConstruct()
{
    Super::NativeConstruct();

    BindViewModel();
}

void UPlayerGoldWidget::NativeDestruct()
{
    UnbindViewModel();

    Super::NativeDestruct();
}

void UPlayerGoldWidget::SetViewModel(UItemViewModel* InViewModel)
{
    UnbindViewModel();
    ItemViewModel = InViewModel;
    BindViewModel();
}

void UPlayerGoldWidget::BindViewModel()
{
    if (ItemViewModel && !bIsViewModelBound)
    {
        ItemViewModel->OnPlayerGoldUpdate.AddDynamic(this, &UPlayerGoldWidget::UpdatePlayerGold);

        bIsViewModelBound = true;
    }
}


void UPlayerGoldWidget::UnbindViewModel()
{
    if (ItemViewModel && bIsViewModelBound)
    {
        ItemViewModel->OnPlayerGoldUpdate.RemoveDynamic(this, &UPlayerGoldWidget::UpdatePlayerGold);

        bIsViewModelBound = false;
    }
}

void UPlayerGoldWidget::UpdatePlayerGold(int32 NewGoldAmount)
{
    if (PlayerGoldText)
    {
        PlayerGoldText->SetText(FText::AsNumber(NewGoldAmount));
    }
}
