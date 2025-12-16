// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterSelect/CharacterPreviewTile.h"
#include "Components/Image.h"
#include "Data/Object/CharacterDataObject.h"

void UCharacterPreviewTile::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	
	// Tile List에서 AddItem(UObject* obj)를 통해 전달된 객체를 받아옴
	// 캐릭터는 종류가 많지 않기에 그냥 캐스팅으로 처리 => 만약에 나중에 종류가 많아지면 인터페이스로 변경 고려
	UCharacterDataObject* CharacterData = Cast<UCharacterDataObject>(ListItemObject);

	if (CharacterData)
	{
		SetCharacterIcon(CharacterData->CharacterIcon);
	}
}

void UCharacterPreviewTile::SetCharacterIcon(UTexture2D* NewIcon)
{
	if (CharacterIcon && NewIcon)
	{
		CharacterIcon->SetBrushFromTexture(NewIcon);
	}
}
