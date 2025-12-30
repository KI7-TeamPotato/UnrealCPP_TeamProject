// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/PlayerStatWidget.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Subsystem/ViewModel/PlayerStatusViewModel.h"
#include "TimerManager.h"

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

void UPlayerStatWidget::SetViewModel(UPlayerStatusViewModel* InViewModel)
{
    UnbindViewModel();
    PlayerStatusViewModel = InViewModel;
    BindViewModel();
}

void  UPlayerStatWidget::BindViewModel()
{
    // 뷰모델의 델리게이트에 바인딩
    if (PlayerStatusViewModel && !bIsViewModelBound)
    {
        // 모델 -> 뷰
        PlayerStatusViewModel->OnPlayerHealthChangedWithText.AddDynamic(this, &UPlayerStatWidget::SetPlayerHealthBar);
        PlayerStatusViewModel->OnPlayerIconChanged.AddDynamic(this, &UPlayerStatWidget::SetPlayerIcon);

        bIsViewModelBound = true;
    }
}

void  UPlayerStatWidget::UnbindViewModel()
{
    if (PlayerStatusViewModel && bIsViewModelBound)
    {
        PlayerStatusViewModel->OnPlayerHealthChangedWithText.RemoveDynamic(this, &UPlayerStatWidget::SetPlayerHealthBar);
        PlayerStatusViewModel->OnPlayerIconChanged.RemoveDynamic(this, &UPlayerStatWidget::SetPlayerIcon);

        bIsViewModelBound = false;
    }
}

void UPlayerStatWidget::SetPlayerHealthBar(float NewHealthPercent, FText NewHealthText)
{
    if (PlayerHealthBar)
    {
        CurrentHealthPercent = NewHealthPercent;
        PlayerHealthBar->SetPercent(CurrentHealthPercent);
    }

    if (HealthText)
    {
        HealthText->SetText(NewHealthText);
    }

    if (CurrentHealthPercent < DelayBarPercent)
    {
        GetWorld()->GetTimerManager().ClearTimer(DelayBarTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(DelayBarWaitTimerHandle);

        // 딜레이 웨이트 타임이 지난 후에 딜레이 바 업데이트 타이머 시작
        GetWorld()->GetTimerManager().SetTimer(DelayBarWaitTimerHandle, this, &UPlayerStatWidget::StartDelayBarTimer, DelayBarWaitTime, false);
    }
    else if (CurrentHealthPercent >= DelayBarPercent)
    {
        // 현재 체력이 딜레이 바보다 크거나 같으면 즉시 동기화
        DelayBarPercent = CurrentHealthPercent;
        if (DelayProgressBar)
        {
            DelayProgressBar->SetPercent(DelayBarPercent);
        }
    }
}

void UPlayerStatWidget::SetPlayerIcon(UTexture2D* NewPlayerIcon)
{
    //UE_LOG(LogTemp, Warning, TEXT("UPlayerStatWidget::SetPlayerIcon - NewPlayerIcon received"));
    if (PlayerIconImage && NewPlayerIcon)
    {
        PlayerIconImage->SetBrushFromTexture(NewPlayerIcon);
    }
}

void UPlayerStatWidget::StartDelayBarTimer()
{
    InterpAlpha = 0.0f;
    // 딜레이 바 업데이트 반복 실행
    GetWorld()->GetTimerManager().SetTimer(DelayBarTimerHandle, this, &UPlayerStatWidget::UpdatePlayerDelayBar, DelayBarInterval, true);
}

void UPlayerStatWidget::UpdatePlayerDelayBar()
{
    //UE_LOG(LogTemp, Warning, TEXT("UPlayerStatWidget::UpdatePlayerDelayBar called,"));
    //UE_LOG(LogTemp, Warning, TEXT("InterpAlpha = %f, CurrentHealthPercent = %f, DelayBarPercent = %f"), InterpAlpha, CurrentHealthPercent, DelayBarPercent);
    if (DelayBarHealthCurve.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("DelayBarHealthCurve is null!"));
    }
    if (InterpAlpha < 1.0f && !DelayBarHealthCurve.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("Updating Delay Bar: InterpAlpha = %f"), InterpAlpha);
        // 커브를 사용하여 보간
        InterpAlpha += DelayBarInterval / InterpDuration;
        InterpAlpha = FMath::Clamp(InterpAlpha, 0.0f, 1.0f);

        float CurveValue = DelayBarHealthCurve->GetFloatValue(InterpAlpha);
        DelayBarPercent = FMath::Lerp(DelayBarPercent, CurrentHealthPercent, CurveValue);

        DelayProgressBar->SetPercent(DelayBarPercent);
    }
    else
    {
        // 딜레이 바가 현재 체력에 도달했으므로 타이머 정지
        GetWorld()->GetTimerManager().ClearTimer(DelayBarTimerHandle);
    }
}

