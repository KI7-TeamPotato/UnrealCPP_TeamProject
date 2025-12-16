// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/MainHud.h"
#include "UI/Perk/PerkSelectionScreenWidget.h"
#include "Subsystem/MVVMSubsystem.h"

void UMainHud::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMainHud::NativeDestruct()
{
	Super::NativeDestruct();
}

//void UMainHud::ShowPerkSelectionScreen()
//{
//    UPerkSelectionScreenWidget* Widget = CreateWidget<UPerkSelectionScreenWidget>(GetOwningPlayerController(), PerkSelectionWidget);
//
//    if (Widget)
//    {
//        // Subsystem 접근은 여기서만! 
//        if (UMVVMSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UMVVMSubsystem>())
//        {
//            Widget->SetViewModel(Subsystem->GetPerkViewModel());
//        }
//
//        Widget->AddToViewport();
//    }
//}