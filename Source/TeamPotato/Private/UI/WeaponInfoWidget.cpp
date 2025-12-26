// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WeaponInfoWidget.h"
#include "Components\TextBlock.h"

void UWeaponInfoWidget::UpdateInfo(float InDamage, float InCost, float InSpeed)
{
    if (DamageText && CostText && SpeedText)
    {
        DamageText->SetText(FText::AsNumber(InDamage));
        CostText->SetText(FText::AsNumber(InCost));
        SpeedText->SetText(FText::AsNumber(InSpeed));
    }
}