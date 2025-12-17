// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryPerkTileWidget.generated.h"

class UTileView;
class UPerkDataAsset;
class UPerkViewModel;

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UInventoryPerkTileWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    // 외부에서 ViewModel 주입
    UFUNCTION(BlueprintCallable, Category = "MVVM")
    void SetViewModel(UPerkViewModel* InViewModel);

	UFUNCTION()
	void LoadPerkDataFromDataAsset(UPerkDataAsset* InData);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnPerkitemHoveredChanged(UObject* Item, bool bIsHovered);

private:
    void BindViewModel();
    void UnbindViewModel();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> PerkTileView = nullptr;

private:
    UPROPERTY()
    TObjectPtr<UPerkViewModel> PerkViewModel;
};
