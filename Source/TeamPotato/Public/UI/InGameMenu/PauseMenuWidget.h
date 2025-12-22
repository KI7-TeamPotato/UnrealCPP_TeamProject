// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UButton* GetContinueButton() const { return ContinueButton; }
    UButton* GetShowCharacterStatButton() const { return ShowCharacterStatButton; }
    UButton* GetSettingButton() const { return SettingButton; }
    UButton* GetGiveUpButton() const { return GiveUpButton; }
    UButton* GetQuitButton() const { return QuitButton; }

protected:
    virtual void NativeConstruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ContinueButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ShowCharacterStatButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> SettingButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> GiveUpButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> QuitButton;
};
