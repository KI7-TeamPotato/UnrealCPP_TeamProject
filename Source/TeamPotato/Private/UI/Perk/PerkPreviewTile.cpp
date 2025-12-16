// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Perk/PerkPreviewTile.h"
#include "Data/Object/PerkDataObject.h"
#include "Components/Image.h"

void UPerkPreviewTile::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UPerkDataObject* PerkDataObject = Cast<UPerkDataObject>(ListItemObject);

	if (PerkDataObject)
	{
		SetPerkIcon(PerkDataObject->PerkIcon);
	}
}

void UPerkPreviewTile::SetPerkIcon(UTexture2D* NewIcon)
{
	if (NewIcon)
	{
		PerkIconImage->SetBrushFromTexture(NewIcon);
	}
}
