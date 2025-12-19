// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemViewModel.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerGoldUpdate, int32, CurrentGold);

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UItemViewModel : public UObject
{
	GENERATED_BODY()
	
public:
    // --- 플레이어의 자원 설정 함수 ---
    UFUNCTION(BlueprintCallable)
    void SetCurrentGold(int32 CurrentGold);

public:
    UPROPERTY(BlueprintAssignable, Category = "Resource")
    FOnPlayerGoldUpdate OnPlayerGoldUpdate;
};
