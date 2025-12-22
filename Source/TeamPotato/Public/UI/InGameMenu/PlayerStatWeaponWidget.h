// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatWeaponWidget.generated.h"

class UWeaponViewModel;
class UImage;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UPlayerStatWeaponWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "MVVM")
    void SetViewModel(UWeaponViewModel* InViewModel);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

private:
    UFUNCTION()
    void UpdateMainWeaponIcon(FText InWeaponName, UTexture2D* InWeaponIcon);
    
    UFUNCTION()
    void UpdateSubWeaponIcon(UTexture2D* InSubWeaponIcon);

    void BindViewModel();
    void UnbindViewModel();

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> MainWeaponIcon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> SubWeaponIcon;

    UPROPERTY()
    TObjectPtr<UWeaponViewModel> WeaponViewModel;

    bool bIsBound = false;
};
