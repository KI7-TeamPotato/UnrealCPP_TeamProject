// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuPlayerStatWidget.generated.h"

class UInventoryPerkTileWidget;
class UPlayerStatWeaponWidget;
class UButton;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UMenuPlayerStatWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UInventoryPerkTileWidget* GetInventoryPerkTileWidget() const { return InventoryPerkTile; }
    UPlayerStatWeaponWidget* GetPlayerWeaponWidget() const { return WeaponInfo; }
    UButton* GetBackButton() const { return BackButton; }

protected:
    virtual void NativeConstruct() override;

private:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UInventoryPerkTileWidget> InventoryPerkTile = nullptr;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UPlayerStatWeaponWidget> WeaponInfo = nullptr;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> BackButton = nullptr;
};

