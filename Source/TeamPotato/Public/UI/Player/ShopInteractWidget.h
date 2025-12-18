// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopInteractWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UShopInteractWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // 가격 세팅 함수
    UFUNCTION(BlueprintCallable)
    void SetPrice(int32 InPrice);

protected:
    // 가격 텍스트
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> PriceText = nullptr;
};
