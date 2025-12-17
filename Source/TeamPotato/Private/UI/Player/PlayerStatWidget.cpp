// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/PlayerStatWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Subsystem/ViewModel/PlayerStatusViewModel.h"

void UPlayerStatWidget::NativeConstruct()
{
	Super::NativeConstruct();

    BindViewModel();
}

void UPlayerStatWidget::NativeDestruct()
{
    UnbindViewModel();

	Super::NativeDestruct();
}

void UPlayerStatWidget::SetPlayerHealthBar(float NewHealthPercent)
{
    if (PlayerHealthBar)
    {
        PlayerHealthBar->SetPercent(NewHealthPercent);
    }
}

void UPlayerStatWidget::SetPlayerResourceBar(float NewResourcePercent)
{
    if (PlayerResourceBar)
    {
        PlayerResourceBar->SetPercent(NewResourcePercent);
    }
}

void UPlayerStatWidget::SetPlayerIcon(UTexture2D* NewPlayerIcon)
{
    if (PlayerIconImage && NewPlayerIcon)
    {
        PlayerIconImage->SetBrushFromTexture(NewPlayerIcon);
    }
}

void UPlayerStatWidget::SetViewModel(UPlayerStatusViewModel* InViewModel)
{
    UnbindViewModel();
    PlayerStatusViewModel = InViewModel;
    BindViewModel();

    // 현재 값으로 UI 초기화
    if (PlayerStatusViewModel)
    {
        SetPlayerHealthBar(PlayerStatusViewModel->GetHealthPercent());
        SetPlayerResourceBar(PlayerStatusViewModel->GetResourcePercent());

        if (PlayerStatusViewModel->GetPlayerIcon())
        {
            SetPlayerIcon(PlayerStatusViewModel->GetPlayerIcon());
        }
    }
}

void  UPlayerStatWidget::BindViewModel()
{
    // 뷰모델의 델리게이트에 바인딩
    if (PlayerStatusViewModel && !bIsViewModelBound)
    {
        // 모델 -> 뷰
        PlayerStatusViewModel->OnPlayerHealthChanged.AddDynamic(this, &UPlayerStatWidget::SetPlayerHealthBar);
        PlayerStatusViewModel->OnPlayerResourceChanged.AddDynamic(this, &UPlayerStatWidget::SetPlayerResourceBar);
        PlayerStatusViewModel->OnPlayerIconChanged.AddDynamic(this, &UPlayerStatWidget::SetPlayerIcon);

        bIsViewModelBound = true;
    }
}

void  UPlayerStatWidget::UnbindViewModel()
{
    if (PlayerStatusViewModel && bIsViewModelBound)
    {
        PlayerStatusViewModel->OnPlayerHealthChanged.RemoveDynamic(this, &UPlayerStatWidget::SetPlayerHealthBar);
        PlayerStatusViewModel->OnPlayerResourceChanged.RemoveDynamic(this, &UPlayerStatWidget::SetPlayerResourceBar);
        PlayerStatusViewModel->OnPlayerIconChanged.RemoveDynamic(this, &UPlayerStatWidget::SetPlayerIcon);

        bIsViewModelBound = false;
    }
}