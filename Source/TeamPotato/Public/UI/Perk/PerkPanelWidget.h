// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PerkPanelWidget.generated.h"

class UTileView;
class UPerkDataAsset;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UPerkPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void LoadPerkDataFromDataAsset(UPerkDataAsset* InData);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnPerkitemHoveredChanged(UObject* Item, bool bIsHovered);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> PerkTileView = nullptr;
};
