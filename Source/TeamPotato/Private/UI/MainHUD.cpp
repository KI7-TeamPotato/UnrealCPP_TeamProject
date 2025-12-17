// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainHUD.h"
#include "UI/Player/MainHUDWidget.h"
#include "Subsystem/MVVMSubsystem.h"

void AMainHUD::BeginPlay()
{
    Super::BeginPlay();

    MainHUDWidget = CreateWidget<UMainHUDWidget>(GetOwningPlayerController(), MainHUDWidgetClass);

    if (MainHUDWidget)
    {
        if (UMVVMSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UMVVMSubsystem>())
        {
            MainHUDWidget->InitializeViewModels(Subsystem);
        }

        MainHUDWidget->AddToViewport();
    }
}