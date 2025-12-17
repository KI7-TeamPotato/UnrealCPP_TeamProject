// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TestUIController.h"
#include "Subsystem/MVVMSubsystem.h"
#include "Subsystem/ViewModel/PerkViewModel.h"
#include "UI/Perk/PerkSelectionScreenWidget.h"

void ATestUIController::BeginPlay()
{
    Super::BeginPlay();

    PerkSelectionScreen = CreateWidget<UPerkSelectionScreenWidget>(this, PerkSelectionScreenClass);

    if (PerkSelectionScreen)
    {
        UE_LOG(LogTemp, Warning, TEXT("PerkSelectionScreen 위젯 생성 완료"));
        if (UMVVMSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UMVVMSubsystem>())
        {
            UPerkViewModel* PerkVM = Subsystem->GetPerkViewModel();
            InitializeViewModels(Subsystem);
            UE_LOG(LogTemp, Warning, TEXT("MVVMSubsystem 획득 완료"));

            PerkVM->OnPerkEquipped.AddDynamic(this, &ATestUIController::RemovePerkSelectionScreenFromViewport);
        }

        // 테스트용, 실제 게임에서는 다른 타이밍에 뷰포트 추가
        PerkSelectionScreen->AddToViewport();
    }
}

void ATestUIController::InitializeViewModels(UMVVMSubsystem* Subsystem)
{
    if (!Subsystem) return;

    // 위젯들에 뷰모델 세팅(이제 위젯은 뷰모델을 알게 된다)
    if (PerkSelectionScreen)
    {
        PerkSelectionScreen->SetViewModel(Subsystem->GetPerkViewModel());
    }
}

// 퍽이 장착되면 호출되어 뷰포트에서 제거
void ATestUIController::RemovePerkSelectionScreenFromViewport(UPerkDataAsset* EquippedPerk)
{
    if (PerkSelectionScreen)
    {
        PerkSelectionScreen->RemoveFromParent();
    }
}

// 델리게이트로 스테이지 클리어 후 뷰포트에 추가 !!!!!!!!!
void ATestUIController::AddPerkSelectionScreenToViewport()
{
    if (PerkSelectionScreen)
    {
        PerkSelectionScreen->AddToViewport();
    }
}
