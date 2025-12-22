// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TestUIController.h"
#include "Subsystem/MVVMSubsystem.h"
#include "Subsystem/ViewModel/PerkViewModel.h"
#include "UI/Perk/PerkSelectionScreenWidget.h"

//void ATestUIController::BeginPlay()
//{
//    Super::BeginPlay();
//
//    PerkSelectionScreen = CreateWidget<UPerkSelectionScreenWidget>(this, PerkSelectionScreenClass);
//
//    if (PerkSelectionScreen)
//    {
//        UE_LOG(LogTemp, Warning, TEXT("PerkSelectionScreen 위젯 생성 완료"));
//        if (UMVVMSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UMVVMSubsystem>())
//        {
//            Subsystem->RegisterPlayerController(this);
//        }
//
//        // 테스트용, 실제 게임에서는 다른 타이밍에 뷰포트 추가
//        PerkSelectionScreen->AddToViewport();
//    }
//}
//
//void ATestUIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
//{
//    if (UMVVMSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UMVVMSubsystem>())
//    {
//        Subsystem->UnregisterPlayerController(this);
//    }
//    Super::EndPlay(EndPlayReason);
//}
//
//// 퍽이 장착되면 호출되어 뷰포트에서 제거
//void ATestUIController::RemovePerkSelectionScreenFromViewport(UPerkDataAsset* EquippedPerk)
//{
//    if (PerkSelectionScreen)
//    {
//        PerkSelectionScreen->RemoveFromParent();
//    }
//}
//
//// 델리게이트로 스테이지 클리어 후 뷰포트에 추가 !!!!!!!!!
//void ATestUIController::AddPerkSelectionScreenToViewport()
//{
//    if (PerkSelectionScreen)
//    {
//        PerkSelectionScreen->AddToViewport();
//    }
//}
