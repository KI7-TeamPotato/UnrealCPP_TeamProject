// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerKilledWidget.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UPlayerKilledWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void OnRestartButtonClicked();

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> RestartButton;
};
