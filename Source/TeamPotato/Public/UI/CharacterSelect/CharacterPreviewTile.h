// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "CharacterPreviewTile.generated.h"

class UImage;
class UChracterDataObject;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UCharacterPreviewTile : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
protected:
    virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
    virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;

	void SetCharacterIcon(UTexture2D* NewIcon);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> CharacterIcon;

private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scale", meta = (AllowPrivateAccess = "true"))
    float SelectedScaleFactor = 1.1f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Color", meta = (AllowPrivateAccess = "true"))
    FLinearColor SelectedColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Color", meta = (AllowPrivateAccess = "true"))
    FLinearColor UnselectedColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.7f);
};
