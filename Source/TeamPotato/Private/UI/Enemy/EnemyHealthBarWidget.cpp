// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Enemy/EnemyHealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "TimerManager.h"

void UEnemyHealthBarWidget::SetHealthPercent(float Percent)
{
    if (EnemyHealthBar)
    {
        CurrentHealthPercent = Percent;
        EnemyHealthBar->SetPercent(CurrentHealthPercent);
    }

    if (DelayProgressBar && 
        CurrentHealthPercent < DelayBarPercent)
    {
        GetWorld()->GetTimerManager().ClearTimer(DelayBarTimerHandle);
        GetWorld()->GetTimerManager().ClearTimer(DelayBarWaitTimerHandle);

        // 딜레이 웨이트 타임이 지난 후에 딜레이 바 업데이트 타이머 시작
        GetWorld()->GetTimerManager().SetTimer(DelayBarWaitTimerHandle, this, &UEnemyHealthBarWidget::StartDelayBarTimer, DelayBarWaitTime, false);
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

void UEnemyHealthBarWidget::StartDelayBarTimer()
{
    InterpAlpha = 0.0f;
    // 딜레이 바 업데이트 반복 실행
    GetWorld()->GetTimerManager().SetTimer(DelayBarTimerHandle, this, &UEnemyHealthBarWidget::UpdateDelayBar, DelayBarInterval, true);
}

void UEnemyHealthBarWidget::UpdateDelayBar()
{
    if (InterpAlpha < 1.0f && DelayBarCurve)
    {
        // 커브를 사용하여 보간
        InterpAlpha += DelayBarInterval / InterpDuration;
        InterpAlpha = FMath::Clamp(InterpAlpha, 0.0f, 1.0f);

        float CurveValue = DelayBarCurve->GetFloatValue(InterpAlpha);
        DelayBarPercent = FMath::Lerp(DelayBarPercent, CurrentHealthPercent, CurveValue);

        DelayProgressBar->SetPercent(DelayBarPercent);
    }
    else
    {
        // 딜레이 바가 현재 체력에 도달했으므로 타이머 정지
        GetWorld()->GetTimerManager().ClearTimer(DelayBarTimerHandle);
    }
}

