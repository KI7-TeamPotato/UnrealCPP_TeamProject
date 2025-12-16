// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/PlayerStatWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Subsystem/MVVMSubsystem.h"
#include "Subsystem/ViewModel/PlayerStatusViewModel.h"

void UPlayerStatWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	UE_LOG(LogTemp, Warning, TEXT("UPlayerStatWidget::NativeConstruct called"));

	PlayerStatusViewModel = GetGameInstance()->GetSubsystem<UMVVMSubsystem>()->GetPlayerStatusViewModel();

	if (PlayerStatusViewModel)
	{
		// 뷰모델의 델리게이트 바인딩
		PlayerStatusViewModel->OnPlayerHealthChanged.AddDynamic(this, &UPlayerStatWidget::SetPlayerHealth);
		PlayerStatusViewModel->OnPlayerResourceChanged.AddDynamic(this, &UPlayerStatWidget::SetPlayerResource);
		PlayerStatusViewModel->OnPlayerIconChanged.AddDynamic(this, &UPlayerStatWidget::SetPlayerIcon);
	}
}

void UPlayerStatWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (PlayerStatusViewModel)
	{
		PlayerStatusViewModel->OnPlayerHealthChanged.RemoveDynamic(this, &UPlayerStatWidget::SetPlayerHealth);
		PlayerStatusViewModel->OnPlayerResourceChanged.RemoveDynamic(this, &UPlayerStatWidget::SetPlayerResource);
		PlayerStatusViewModel->OnPlayerIconChanged.RemoveDynamic(this, &UPlayerStatWidget::SetPlayerIcon);
	}
}

void UPlayerStatWidget::SetPlayerHealth(float NewHealthPercent)
{
	UE_LOG(LogTemp, Warning, TEXT("SetPlayerHealth called with percent: %f"), NewHealthPercent);
	PlayerHealthBar->SetPercent(NewHealthPercent);
}

void UPlayerStatWidget::SetPlayerResource(float NewResourcePercent)
{
	PlayerResourceBar->SetPercent(NewResourcePercent);
}

void UPlayerStatWidget::SetPlayerIcon(UTexture2D* NewPlayerIcon)
{
	PlayerIconImage->SetBrushFromTexture(NewPlayerIcon);
}