// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ConfirmPopupWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConfirmPopupConfirmed);

class UButton;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UConfirmPopupWidget : public UUserWidget
{
	GENERATED_BODY()
	

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UFUNCTION()
    void OnConfirmButtonClicked();
    
    UFUNCTION()
    void OnCancelButtonClicked();
public:
    UPROPERTY(BlueprintAssignable, Category = "ConfirmPopup")
    FOnConfirmPopupConfirmed OnConfirmed;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> ConfirmButton;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> CancelButton;
};
