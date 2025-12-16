// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Perk/PerkCardWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/PerkDataAsset.h"

void UPerkCardWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SelectButton->OnClicked.AddDynamic(this, &UPerkCardWidget::OnPerkSelectButtonClicked);
}

void UPerkCardWidget::NativeDestruct()
{
    SelectButton->OnClicked.RemoveDynamic(this, &UPerkCardWidget::OnPerkSelectButtonClicked);

    Super::NativeDestruct();
}

void UPerkCardWidget::SetupPerkCard(UPerkDataAsset* InPerkDataAsset)
{
    if (!InPerkDataAsset) return;

    PerkDataAsset = InPerkDataAsset;

    PerkImage->SetBrushFromTexture(PerkDataAsset->Icon);
    PerkName->SetText(PerkDataAsset->PerkName);
    PerkDescript->SetText(PerkDataAsset->Description);
}

void UPerkCardWidget::OnPerkSelectButtonClicked()
{
    //if (OnTryEquippedPerk.IsBound() && SelectedPerk)
    //{
    //    OnTryEquippedPerk.Broadcast(SelectedPerk);
    //}
}
