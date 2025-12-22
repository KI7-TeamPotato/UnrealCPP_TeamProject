// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TestPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubSystems.h"
#include "InputMappingContext.h"
#include "Subsystem/MVVMSubsystem.h"
#include "UI/InGameMenu/InGameMenuWidget.h"
#include "UI/InGameMenu/PlayerStatWeaponWidget.h"
#include "UI/Perk/PerkSelectionScreenWidget.h"
#include "UI/InGameMenu/MenuPlayerStatWidget.h"
#include "UI/Perk/InventoryPerkTileWidget.h"
#include "Kismet/GameplayStatics.h"

void ATestPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* Subsystem =	ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (Subsystem)
	{
		UE_LOG(LogTemp, Log, TEXT("Player Controller Subsystem Available"));
		Subsystem->AddMappingContext(DefaultMappingContext, priority);
	}

    // 인게임 메뉴 위젯 생성
    if (InGameMenuWidgetClass)
    {
       InGameMenuWidget =
            CreateWidget<UInGameMenuWidget>(this, InGameMenuWidgetClass);
        InGameMenuWidget->AddToViewport(10);
        InGameMenuWidget->SetVisibility(ESlateVisibility::Hidden);
    }

    // 퍽 선택 화면 위젯 바인딩 && 퍽 인벤토리 뷰모델 설정
    if (PerkSelectionScreenClass)
    {
        PerkSelectionScreen = CreateWidget<UPerkSelectionScreenWidget>(this, PerkSelectionScreenClass);

        if (UMVVMSubsystem* MVVMSubsystem = GetGameInstance()->GetSubsystem<UMVVMSubsystem>())
        {
            PerkSelectionScreen->SetViewModel(MVVMSubsystem->GetPerkViewModel());

            if (InGameMenuWidget)
            {
                // 인게임 메뉴의 플레이어 정보 패널 델리게이트
                InGameMenuWidget->GetPlayingPlayerStatPanel()
                    ->GetInventoryPerkTileWidget()
                    ->SetViewModel(MVVMSubsystem->GetPerkViewModel());

                InGameMenuWidget->GetPlayingPlayerStatPanel()
                    ->GetPlayerWeaponWidget()
                    ->SetViewModel(MVVMSubsystem->GetWeaponViewModel());

                // 계속하기 버튼 처리
                InGameMenuWidget->OnInGameMenuClosed.AddDynamic(this, &ATestPlayerController::OnPauseInput);
            }
        }

        // 테스트용, 실제 게임에서는 다른 타이밍에 뷰포트 추가
        PerkSelectionScreen->AddToViewport();
    }
}

void ATestPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        if (IA_Pause)
        {
            EnhancedInput->BindAction(IA_Pause, ETriggerEvent::Started, this, &ATestPlayerController::OnPauseInput);
        }
    }

}

void ATestPlayerController::OnPauseInput()
{
    if (bIsMenuOpen && InGameMenuWidget)
    {
        // 메뉴 닫기
        // 게임 재개
        SetPause(false);

        FInputModeGameOnly InputMode;
        InputMode.SetConsumeCaptureMouseDown(false);
        SetInputMode(InputMode);
        SetShowMouseCursor(false);

        InGameMenuWidget->SetVisibility(ESlateVisibility::Collapsed);

        bIsMenuOpen = false;
        InGameMenuWidget->InitializePauseMenu(); //메뉴 초기화
    }
    else
    {
        //메뉴 열기
        //게임 정지
        SetPause(true);

        // Ui에 입력 포커스 설정
        FInputModeGameAndUI InputMode;
        InputMode.SetWidgetToFocus(InGameMenuWidget->TakeWidget());
        SetInputMode(InputMode);
        SetShowMouseCursor(true);

        InGameMenuWidget->SetVisibility(ESlateVisibility::Visible);

        bIsMenuOpen = true;
    }
}

// 델리게이트로 스테이지 클리어 후 뷰포트에 추가 !!!!!!!!!
void ATestPlayerController::AddPerkSelectionScreenToViewport()
{
    if (PerkSelectionScreen)
    {
        PerkSelectionScreen->AddToViewport();
    }
}