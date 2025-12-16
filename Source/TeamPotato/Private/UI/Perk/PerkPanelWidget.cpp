// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Perk/PerkPanelWidget.h"
#include "Data/Object/PerkDataObject.h"
#include "Subsystem/MVVMSubsystem.h"
#include "Subsystem/ViewModel/SkillViewModel.h"
#include "Components/TileView.h"

void UPerkPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PerkTileView)
	{
		PerkTileView->ClearListItems();
		PerkTileView->OnItemIsHoveredChanged().AddUObject(this, &UPerkPanelWidget::OnPerkitemHoveredChanged);
	}

	// mvvm서브 시스템에서 패시브 스킬을 얻으면 불러올
	USkillViewModel* SkillViewModel = GetGameInstance()->GetSubsystem<UMVVMSubsystem>()->GetSkillViewModel();

	if (SkillViewModel)
	{
		// 뷰모델의 델리게이트 바인딩
		SkillViewModel->OnPerkDataAssetChanged.AddDynamic(this, &UPerkPanelWidget::LoadPerkDataFromDataAsset);
	}
}

void UPerkPanelWidget::NativeDestruct()
{
	if (PerkTileView)
	{
		PerkTileView->OnItemIsHoveredChanged().RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UPerkPanelWidget::LoadPerkDataFromDataAsset(UPerkDataAsset* InData)
{
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
