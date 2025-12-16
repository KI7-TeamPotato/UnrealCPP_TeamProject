// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PerkSelectionWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTryEquippedPerk, UPerkDataAsset*, NewPerkDataAsset);

class UButton;
class UDataTable;
class UPerkDataAsset;
struct FPerkSelectDataTableRow;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UPerkSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	UButton* GetPerkSelectButton1() const { return PerkSelectButton1; }

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnPerkSelectButtonClicked();

private:
	void SetupPerkButtons();
	TArray<FPerkSelectDataTableRow> GetRandomPerkDataRows(UDataTable* InDataTable, int32 NumSelect);

public:
	UPROPERTY()
	FOnTryEquippedPerk OnTryEquippedPerk;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PerkSelectButton1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PerkSelectButton2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PerkSelectButton3;

private:
	UPROPERTY(EditAnywhere, Category = "Perk", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> PerkDataRow;

	UPROPERTY(EditAnywhere, Category = "Perk", meta = (AllowPrivateAccess = "true"))
	int32 PerkNum=3;

	UPROPERTY(VisibleAnywhere, Category = "Perk", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPerkDataAsset> SelectedPerk;
};
