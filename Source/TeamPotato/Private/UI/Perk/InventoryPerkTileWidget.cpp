// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Perk/PerkPanelWidget.h"
#include "Data/Object/PerkDataObject.h"
#include "Subsystem/ViewModel/PerkViewModel.h"
#include "Components/TileView.h"

void UPerkPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PerkTileView)
	{
		PerkTileView->ClearListItems();
		PerkTileView->OnItemIsHoveredChanged().AddUObject(this, &UPerkPanelWidget::OnPerkitemHoveredChanged);
	}

    BindViewModel();
}

void UPerkPanelWidget::NativeDestruct()
{
	if (PerkTileView)
	{
		PerkTileView->OnItemIsHoveredChanged().RemoveAll(this);
	}

    UnbindViewModel();

	Super::NativeDestruct();
}

void UPerkPanelWidget::LoadPerkDataFromDataAsset(UPerkDataAsset* InData)
{
    if (!InData || !PerkTileView) return;

	UPerkDataObject* PerDataObject = NewObject<UPerkDataObject>(this);
	PerDataObject->InitFromDataTableAsset(InData);
	PerkTileView->AddItem(PerDataObject);
}


void UPerkPanelWidget::OnPerkitemHoveredChanged(UObject* Item, bool bIsHovered)
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

void UPerkPanelWidget::SetViewModel(UPerkViewModel* InViewModel)
{
    UnbindViewModel();
    PerkViewModel = InViewModel;
    BindViewModel();
}

void UPerkPanelWidget::BindViewModel()
{
    if (PerkViewModel)
    {
        // Model -> ViewModel 바인딩
        PerkViewModel->OnPerkEquipped.AddDynamic(this, &UPerkPanelWidget::LoadPerkDataFromDataAsset);
    }
}
void UPerkPanelWidget::UnbindViewModel()
{
    if (PerkViewModel)
    {
        PerkViewModel->OnPerkEquipped.RemoveDynamic(this, &UPerkPanelWidget::LoadPerkDataFromDataAsset);
    }
}