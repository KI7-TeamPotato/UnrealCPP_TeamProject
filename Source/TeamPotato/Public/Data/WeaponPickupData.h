// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WeaponPickupData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FWeaponPickupData : public FTableRowBase
{
    GENERATED_BODY()

public:
    // 픽업 액터 클래스
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    TObjectPtr<class UWeaponDataAsset> WeaponData;

    // 등장 확률 가중치
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    float WeaponWeight = 1.0f;
};
