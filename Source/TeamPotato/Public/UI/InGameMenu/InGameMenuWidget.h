// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenuWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInGameMenuClosed);

class UWidgetSwitcher;
class UPauseMenuWidget;
class UMenuPlayerStatWidget;
class UMainMenuSoundOptionWidget;
class UConfirmPopupWidget;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UInGameMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION()
    void InitializePauseMenu();
	
    UFUNCTION()
    UMenuPlayerStatWidget* GetPlayingPlayerStatPanel() const { return PlayingPlayerStatPanel; }

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void OnContinueButtonClicked();
    UFUNCTION()
    void OnShowCharacterStatButtonClicked();
    UFUNCTION()
    void OnSettingButtonClicked();
    UFUNCTION()
    void OnGiveUpButtonClicked();
    UFUNCTION()
    void OnQuitButtonClicked();
    
    // 포기 확인 팝업 응답 처리
    UFUNCTION()
    void OnConfirmGiveUp();

    // 종료 확인 팝업 응답 처리
    UFUNCTION()
    void OnConfirmQuit();

    // 패널들의 백버튼 클릭 처리
    UFUNCTION()
    void OnBackToPauseClicked();

public:
    UPROPERTY(BlueprintAssignable, Category = "InGameMenu")
    FOnInGameMenuClosed OnInGameMenuClosed;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWidgetSwitcher> MenuWidgetSwitcher = nullptr;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UPauseMenuWidget> PauseMenuPanel = nullptr;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UMenuPlayerStatWidget> PlayingPlayerStatPanel = nullptr;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UMainMenuSoundOptionWidget> InGameSoundOptionMenuPanel = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InGameMenu")
    TSubclassOf<UConfirmPopupWidget> ConfirmPopupWidgetClass = nullptr;
};
