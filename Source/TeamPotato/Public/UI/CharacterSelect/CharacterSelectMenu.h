// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/CharacterDataTableRow.h"
#include "CharacterSelectMenu.generated.h"

class UCharacterDataObject;
class UCharacterSubsystem;
class UCharacterSelectDetailWidget;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UCharacterSelectMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnCharacterPreviewTileSelected(UObject* SelectedItem);	

	UFUNCTION()
	void OnSelectButtonClicked();

private:
	void LoadCharacterDataFromSubsystem();

	void SetCharacterIllustration(UTexture2D* InIllustration);

	UCharacterSubsystem* GetCharacterSubsystem() const;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UTileView> CharacterTileView = nullptr;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UButton> SelectButton = nullptr;
    
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UCharacterSelectDetailWidget> CharacterSelectDetailPanel = nullptr;

private:
	UPROPERTY()
	TObjectPtr<UCharacterDataObject> SelectedCharacterData = nullptr;
};
