// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponInfoWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UWeaponInfoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Data", meta = (BindWidget))
    TObjectPtr<UTextBlock> DamageText = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Data", meta = (BindWidget))
    TObjectPtr<UTextBlock> CostText = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Data", meta = (BindWidget))
    TObjectPtr<UTextBlock> SpeedText = nullptr;

    // 데이터를 한 번에 설정하는 함수
    void UpdateInfo(float InDamage, float InCost, float InSpeed);
};
