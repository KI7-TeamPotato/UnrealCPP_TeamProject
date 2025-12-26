// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "TestPlayerController.generated.h"

class UInputMappingContext;
class UPerkSelectionScreenWidget;
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
    void RemovePerkSelectionScreenFromViewport();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void SetGameOnlyInputMode();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void SetUIOnlyInputMode();

    UFUNCTION(BlueprintCallable, Category = "Input")
    void SetGameAndUIInputMode();


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

protected:
    //IA
    // 일시 정지 입력
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction")
    TObjectPtr<UInputAction> IA_Pause = nullptr;

private:
	int32 priority = 1;

    // --- 메뉴 관련 변수 --- 
    bool bIsMenuOpen = false;

    UPROPERTY()
    TObjectPtr<UInGameMenuWidget> InGameMenuWidget = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = "true"))
    TSubclassOf<UInGameMenuWidget> InGameMenuWidgetClass;

    // --- 퍽 관련 변수 ---
    UPROPERTY()
    TObjectPtr<UPerkSelectionScreenWidget> PerkSelectionScreen;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UPerkSelectionScreenWidget> PerkSelectionScreenClass;
};
