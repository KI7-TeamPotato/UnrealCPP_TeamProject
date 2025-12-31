// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "TestPlayerController.generated.h"

class UInputMappingContext;
class UPerkSelectionScreenWidget;
class UPlayerKilledWidget;
class UInGameMenuWidget;
class UPerkDataAsset;

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API ATestPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
    // 선택창을 뷰포트에 추가
    UFUNCTION()
    void AddPerkSelectionScreenToViewport();

    UFUNCTION()
    void RemovePerkSelectionScreenFromViewport(UPerkDataAsset* _EquippedPerk);

    UFUNCTION(BlueprintCallable, Category = "Input")
    void SetGameOnlyInputMode();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void SetUIOnlyInputMode();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void SetGameAndUIInputMode();

    // 스테이지와 챕터 정보를 받아서 원하는 스테이지와 챕터에서 퍽 선택 화면을 띄움
    UFUNCTION(BlueprintCallable, Category = "Perk|Selection")
    void TryPerkSelectionScreen(int32 InStage, int32 InChapter);

protected:
    // --- 빙의 시점에 강제로 Input을 GameModeOnly로 바꿈 ---
    virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction")
	TObjectPtr<UInputMappingContext> DefaultMappingContext = nullptr;

    // --- 일시 정지 후 메뉴 처리 ---
    UFUNCTION()
    void OnPauseInput();

private:
    UFUNCTION()
    void OnAddPlayerKilledWidget();

protected:
    //IA
    // 일시 정지 입력
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction")
    TObjectPtr<UInputAction> IA_Pause = nullptr;

private:
	int32 priority = 1;

    // --- 메뉴 UI 관련 변수 --- 
    bool bIsMenuOpen = false;

    UPROPERTY()
    TObjectPtr<UInGameMenuWidget> InGameMenuWidget = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UInGameMenuWidget> InGameMenuWidgetClass;

    // --- 퍽 UI 관련 변수 ---
    UPROPERTY()
    TObjectPtr<UPerkSelectionScreenWidget> PerkSelectionScreen;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UPerkSelectionScreenWidget> PerkSelectionScreenClass;

    // --- 사망 UI 위젯 관련 변수 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UPlayerKilledWidget> PlayerKilledWidget;

    //UPROPERTY(EditDefaultsOnly, Category = "UI")
    //TSubclassOf<UPlayerKilledWidget> PlayerKilledWidgetClass;
};
