// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "InventoryPerkPreviewTile.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UInventoryPerkPreviewTile : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
protected:
	UFUNCTION()
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	void SetPerkIcon(UTexture2D* NewIcon);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PerkIconImage;
};
