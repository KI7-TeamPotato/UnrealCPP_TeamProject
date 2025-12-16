// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PerkSelectionScreenWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTryEquippedPerk, UPerkDataAsset*, NewPerkDataAsset);

class UPerkCardWidget;
class UDataTable;
class UPerkDataAsset;
struct FPerkSelectDataTableRow;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UPerkSelectionScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
    UPerkCardWidget* GetPerkCard1() const { return PerkCard1; }

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
	TObjectPtr<UPerkCardWidget> PerkCard1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPerkCardWidget> PerkCard2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPerkCardWidget> PerkCard3;

private:
	UPROPERTY(EditAnywhere, Category = "Perk", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> PerkDataRow;

	UPROPERTY(EditAnywhere, Category = "Perk", meta = (AllowPrivateAccess = "true"))
	int32 PerkNum=3;

	UPROPERTY(VisibleAnywhere, Category = "Perk", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPerkDataAsset> SelectedPerk;
};
