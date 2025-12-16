// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Perk/InventoryPerkPreviewTile.h"
#include "Data/Object/PerkDataObject.h"
#include "Components/Image.h"

void UInventoryPerkPreviewTile::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UPerkDataObject* PerkDataObject = Cast<UPerkDataObject>(ListItemObject);

	if (PerkDataObject)
	{
		SetPerkIcon(PerkDataObject->PerkIcon);
	}
}

void UInventoryPerkPreviewTile::SetPerkIcon(UTexture2D* NewIcon)
{
	if (NewIcon)
	{
		PerkIconImage->SetBrushFromTexture(NewIcon);
	}
}
