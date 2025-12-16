// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/MainHUDWidget.h"
#include "Subsystem/MVVMSubsystem.h"
#include "UI/Player/PlayerStatWidget.h"

void UMainHUDWidget::InitializeViewModels(UMVVMSubsystem* Subsystem)
{
    if (!Subsystem) return;

    // 위젯들에 뷰모델 세팅(이제 위젯은 뷰모델을 알게 된다)
    if (PlayerStatPanel)
    {
        PlayerStatPanel->SetViewModel(Subsystem->GetPlayerStatusViewModel());
    }
}
