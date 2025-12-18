// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Minimap/MinimapWidget.h"
#include "Components/Image.h"
#include "Subsystem/ViewModel/MinimapViewModel.h"

void UMinimapWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    BindViewModel();
}

void UMinimapWidget::NativeDestruct()
{
    UnbindViewModel();

    Super::NativeDestruct();
}

void UMinimapWidget::UpdatePlayerIconPosition(const FVector2D& NewPosition)
{

}

void UMinimapWidget::SetViewModel(UMinimapViewModel* InViewModel)
{
    UnbindViewModel();
    MinimapViewModel = InViewModel;
    BindViewModel();
}

void UMinimapWidget::BindViewModel()
{
    if (MinimapViewModel && !bIsViewModelBound)
    {
        bIsViewModelBound = true;
    }
}

void UMinimapWidget::UnbindViewModel()
{
    if (MinimapViewModel && bIsViewModelBound)
    {
        bIsViewModelBound = false;
    }
}