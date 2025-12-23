// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/PerkDataAsset.h"
#include "PerkComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerkStateChanged, UPerkDataAsset*, InData);

class UPerkDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMPOTATO_API UPerkComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPerkComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	// --- 1.Equipment ---

	// Equipment - EquipPerk
	UFUNCTION(BlueprintCallable, Category = "Perk System")
	bool EquipPerk(UPerkDataAsset* PerkToEquip, int32 SlotIndex = -1);

	// Equipment - UnequipPerk
	UFUNCTION(BlueprintCallable, Category = "Perk System")
	void UnequipPerk(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Perk System")
	void ClearAllEquippedPerks();

	// Check if Same Perk is Equipped
	UFUNCTION(BlueprintPure, Category = "Perk System")
	bool IsPerkEquipped(UPerkDataAsset* PerkToCheck) const;

	// Equipment - Swap Equipped Perks (더 높은 레어리티로 정렬을 위한 함수)
	UFUNCTION(BlueprintCallable, Category = "Perk System")
	bool SwapEquippedPerks(int32 FromIndex, int32 ToIndex);

	// Equipment - getter for a specific slot
	UFUNCTION(BlueprintPure, Category = "Perk System")
	UPerkDataAsset* GetEquippedPerkAt(int32 SlotIndex) const;

	// Equipment - getter for all equipped perks
	UFUNCTION(BlueprintPure, Category = "Perk System")
	const TArray<UPerkDataAsset*>& GetEquippedPerks() const { return EquippedPerks; }

	//=========================================================================================//
	//=======================================Value=============================================//
	//=========================================================================================//
public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPerkStateChanged OnEquipmentUpdated;

protected:
	// --- Equipment Slot Array ---
	UPROPERTY(VisibleAnywhere, Category = "State")
	TArray<TObjectPtr<UPerkDataAsset>> EquippedPerks;

	// ---  Array of Activated Effect Instances (for managing active effects) ---
	// Key : SlotIndex, Value : Affect Instance
	UPROPERTY(VisibleAnywhere, Category = "State")
	TMap <int32, TObjectPtr<class UPerkEffectBase>> ActivateEffects;

	// --- Config ---
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	int32 MaxEquipSlots = 4;
};
