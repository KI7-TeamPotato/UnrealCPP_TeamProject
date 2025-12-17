// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/PerkDataAsset.h"
#include "PerkViewModel.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerkEquipped, UPerkDataAsset*, NewPerkDataAsset);
DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(bool, FOnEquipPerkRequest, UPerkDataAsset*, PerkToEquip, int32, InIndex);
/**
 * 
 */
UCLASS(Blueprintable)
class TEAMPOTATO_API UPerkViewModel : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetPerkDataAsset(UPerkDataAsset* NewData);

    // 퍽 장착 요청(View to Model)
	UFUNCTION(BlueprintCallable)
    bool RequestEquipPerk(UPerkDataAsset* NewData, int32 SlotIndex = -1);

	// --- 컴포넌트 -> PerPanelWidget ---
	UPROPERTY(BlueprintAssignable)
	FOnPerkEquipped OnPerkEquipped;
	
	UPROPERTY()
	FOnEquipPerkRequest OnEquipPerkRequest;

private:
    UPROPERTY()
    TArray<TObjectPtr<UPerkDataAsset>> CachedEquippedPerks;
};
