// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UMainMenuButtonWidget;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void OnNewGameClicked();
	//UFUNCTION(BlueprintCallable, Category = "MainMenu")
	//void OnContinueGameClicked();
	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void OnSettingClicked();
	UFUNCTION(BlueprintCallable, Category = "MainMenu")
	void OnQuitClicked();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UMainMenuButtonWidget> NewGameButton;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	//TObjectPtr<UMainMenuButtonWidget> ContinueGameButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UMainMenuButtonWidget> SettingsButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UMainMenuButtonWidget> QuitButton;
};
