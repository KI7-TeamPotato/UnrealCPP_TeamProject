// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Player/ShopInteractWidget.h"
#include "Components\TextBlock.h"

void UShopInteractWidget::SetPrice(int32 InPrice)
{
    if (PriceText)
    {
        PriceText->SetText(FText::AsNumber(InPrice));
    }
}
