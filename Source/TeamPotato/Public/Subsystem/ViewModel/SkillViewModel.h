// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/PerkDataAsset.h"
#include "SkillViewModel.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerkDataAssetChanged, UPerkDataAsset*, NewPerkDataAsset);
DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(bool, FOnEquipPerkRequest, UPerkDataAsset*, PerkToEquip, int32, InIndex);
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API USkillViewModel : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetPerkDataAsset(UPerkDataAsset* NewData);

	UFUNCTION(BlueprintCallable)
	void EquippedPerkInPerkComp(UPerkDataAsset* NewData);

	// --- 컴포넌트 -> PerPanelWidget ---
	UPROPERTY(BlueprintAssignable)
	FOnPerkDataAssetChanged OnPerkDataAssetChanged;
	
	UPROPERTY()
	FOnEquipPerkRequest OnEquipPerkRequest;
};
