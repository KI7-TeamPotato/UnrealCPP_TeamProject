// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UMainMenuButtonWidget;
class UWidgetSwitcher;
class UMainMenuSoundOptionWidget;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void OnNewGameClicked();

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void OnSettingClicked();

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void OnQuitClicked();

private:
    UFUNCTION()
    void OnSoundOptionCloseClicked();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UMainMenuButtonWidget> NewGameButton = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UMainMenuButtonWidget> SettingsButton = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UMainMenuButtonWidget> QuitButton = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UWidgetSwitcher> MenuWidgetSwitcher = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UMainMenuSoundOptionWidget> MainSoundOptionPanel = nullptr;
};
