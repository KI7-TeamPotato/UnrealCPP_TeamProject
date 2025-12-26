// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterSelect/CharacterPreviewTile.h"
#include "Components/Image.h"
#include "Data/Object/CharacterDataObject.h"

void UCharacterPreviewTile::NativeConstruct()
{
    Super::NativeConstruct();

    this->SetRenderScale(FVector2D(1.f, 1.f));
    this->SetRenderOpacity(UnselectedColor.A);
}

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

void UCharacterPreviewTile::NativeOnItemSelectionChanged(bool bIsSelected)
{
    IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);

    if (bIsSelected)
    {
        // 선택 되었다면 크기를 1.1f 키우고 알파 값을 1.0f로 설정
        this->SetRenderScale(FVector2D(SelectedScaleFactor, SelectedScaleFactor));
        this->SetRenderOpacity(SelectedColor.A);
    }
    else
    {
        // 선택 해제 되었다면 크기를 원래대로 돌리고 알파 값을 0.7f로 설정
        this->SetRenderScale(FVector2D(1.f, 1.f));
        this->SetRenderOpacity(UnselectedColor.A);
    }
}

void UCharacterPreviewTile::SetCharacterIcon(UTexture2D* NewIcon)
{
	if (CharacterIcon && NewIcon)
	{
		CharacterIcon->SetBrushFromTexture(NewIcon);
	}
}
