// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Perk/PerkCardWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/PerkDataAsset.h"

void UPerkCardWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (SelectButton)
    {
        SelectButton->OnClicked.AddDynamic(this, &UPerkCardWidget::OnPerkSelectButtonClicked);
    }
}

void UPerkCardWidget::NativeDestruct()
{
    if (SelectButton)
    {
        SelectButton->OnClicked.RemoveDynamic(this, &UPerkCardWidget::OnPerkSelectButtonClicked);
    }
    Super::NativeDestruct();
}

void UPerkCardWidget::SetupPerkCard(UPerkDataAsset* InPerkDataAsset)
{
    if (!InPerkDataAsset) return;

    PerkDataAsset = InPerkDataAsset;
    
    if (PerkImage && PerkDataAsset->Icon)
    {
        PerkImage->SetBrushFromTexture(PerkDataAsset->Icon);
    }
    if (PerkName)
    {
        PerkName->SetText(PerkDataAsset->PerkName);
    }
    if (PerkDescript)
    {
        PerkDescript->SetText(PerkDataAsset->Description);
    }
}

void UPerkCardWidget::OnPerkSelectButtonClicked()
{
    if (PerkDataAsset)
    {
        UE_LOG(LogTemp, Log, TEXT("PerkCardWidget:  Perk Selected - %s"), *PerkDataAsset->PerkName.ToString());
        OnPerkCardSelected.Broadcast(PerkDataAsset);
    }
}
