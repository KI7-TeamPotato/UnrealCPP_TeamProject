// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Perk/PerkCardWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Data/PerkDataAsset.h"
#include "Data/EPerkRarity.h"
#include "Data/PerkHelperAsset.h"

void UPerkCardWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (SelectButton)
    {
        // 버튼 클릭 및 호버 이벤트 바인딩
        SelectButton->OnClicked.AddDynamic(this, &UPerkCardWidget::OnPerkSelectButtonClicked);
        SelectButton->OnHovered.AddDynamic(this, &UPerkCardWidget::OnPerkButtonHovered);
        SelectButton->OnUnhovered.AddDynamic(this, &UPerkCardWidget::OnPerkButtonUnhovered);
    }

    if (BackgroundImage)
    {
        BackgroundImage->SetContentColorAndOpacity(DefaultBackgroundColor);
        BackgroundImage->SetBrushColor(DefaultBackgroundColor);
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
    if (BackgroundImage)
    {
        UTexture2D* PerkFrame = PerkHelperAsset->GetPerkFrameByRarity(InPerkDataAsset->PerkRarity);
        BackgroundImage->SetBrushFromTexture(PerkFrame);
    }
    if (PerkName)
    {
        PerkName->SetText(PerkDataAsset->PerkName);
    }
    if (PerkDescript)
    {
        FText FormattedDescription = PerkDataAsset->GetFormattedDescription();
        PerkDescript->SetText(FormattedDescription);
    }
}

void UPerkCardWidget::OnPerkSelectButtonClicked()
{
    if (PerkDataAsset)
    {
        // 버튼 클릭 시 뷰(This) -> 모델(PerkComponent) 전달
        UE_LOG(LogTemp, Log, TEXT("PerkCardWidget:  Perk Selected - %s"), *PerkDataAsset->PerkName.ToString());
        OnPerkCardSelected.Broadcast(PerkDataAsset);
    }
}

void UPerkCardWidget::OnPerkButtonHovered()
{
    if (BackgroundImage)
    {
        BackgroundImage->SetContentColorAndOpacity(HoveredBackgroundColor);
        BackgroundImage->SetBrushColor(HoveredBackgroundColor);
    }
}

void UPerkCardWidget::OnPerkButtonUnhovered()
{
    if (BackgroundImage)
    {
        BackgroundImage->SetContentColorAndOpacity(DefaultBackgroundColor);
        BackgroundImage->SetBrushColor(DefaultBackgroundColor);
    }
}
