// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainHUD.h"
#include "UI/Player/MainHUDWidget.h"
#include "Subsystem/MVVMSubsystem.h"

void AMainHUD::BeginPlay()
{
    Super::BeginPlay();

    // 메인 HUD 위젯 생성
    MainHUDWidget = CreateWidget<UMainHUDWidget>(GetOwningPlayerController(), MainHUDWidgetClass);

    // 메인 HUD 위젯 초기화 및 뷰포트에 추가
    if (MainHUDWidget)
    {
        if (UMVVMSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UMVVMSubsystem>())
        {
            MainHUDWidget->InitializeViewModels(Subsystem);
        }

        MainHUDWidget->AddToViewport();
    }
}

void AMainHUD::TryShowBossWidget()
{
    if (MainHUDWidget)
    {
        if (UMVVMSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UMVVMSubsystem>())
        {
            MainHUDWidget->ShowBossWidget(Subsystem);
        }
    }
}

void AMainHUD::TryHideBossWidget()
{
    if (MainHUDWidget)
    {
        if (UMVVMSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UMVVMSubsystem>())
        {
            MainHUDWidget->HideBossWidget(Subsystem);
        }
    }
}
