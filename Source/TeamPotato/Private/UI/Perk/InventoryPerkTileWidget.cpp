// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Perk/InventoryPerkTileWidget.h"
#include "Data/Object/PerkDataObject.h"
#include "Subsystem/ViewModel/PerkViewModel.h"
#include "Components/TileView.h"

void UInventoryPerkTileWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PerkTileView)
	{
		PerkTileView->ClearListItems();
		PerkTileView->OnItemIsHoveredChanged().AddUObject(this, &UInventoryPerkTileWidget::OnPerkitemHoveredChanged);
	}
}

void UInventoryPerkTileWidget::NativeDestruct()
{
	if (PerkTileView)
	{
		PerkTileView->OnItemIsHoveredChanged().RemoveAll(this);
	}

    UnbindViewModel();

	Super::NativeDestruct();
}

void UInventoryPerkTileWidget::LoadPerkDataFromDataAsset(UPerkDataAsset* InData)
{
    if (!InData || !PerkTileView) return;

    UE_LOG(LogTemp, Warning, TEXT("InventoryPerkTileWidget::LoadPerkDataFromDataAsset - 퍽 데이터 로드 시작"));
    UE_LOG(LogTemp, Warning, TEXT("My Widget Name: %s"), *this->GetName());
    

	UPerkDataObject* PerDataObject = NewObject<UPerkDataObject>(this);
	PerDataObject->InitFromDataTableAsset(InData);
	PerkTileView->AddItem(PerDataObject);
}


void UInventoryPerkTileWidget::OnPerkitemHoveredChanged(UObject* Item, bool bIsHovered)
{
	if (Item)
	{
		// 설명창 내용을 업데이트
	}

	if (bIsHovered)
	{
		//설명창 띄우기
	}
}

void UInventoryPerkTileWidget::SetViewModel(UPerkViewModel* InViewModel)
{
    UnbindViewModel();
    PerkViewModel = InViewModel;
    BindViewModel();
}

void UInventoryPerkTileWidget::BindViewModel()
{
    if (PerkViewModel)
    {
        // Model -> ViewModel 바인딩
        PerkViewModel->OnPerkEquipped.AddDynamic(this, &UInventoryPerkTileWidget::LoadPerkDataFromDataAsset);
    }
}
void UInventoryPerkTileWidget::UnbindViewModel()
{
    if (PerkViewModel)
    {
        PerkViewModel->OnPerkEquipped.RemoveDynamic(this, &UInventoryPerkTileWidget::LoadPerkDataFromDataAsset);
    }
}