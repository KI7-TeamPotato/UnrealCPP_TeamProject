// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.generated.h"

class UMVVMSubsystem;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "UI|MainHud")
    void InitializeViewModels(UMVVMSubsystem* Subsystem);

protected:
	// --- 플레이어 스탯 위젯 ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UPlayerStatWidget> PlayerStatPanel;

    // --- 인벤토리 퍽 위젯 ---
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UInventoryPerkTileWidget> InventoryPerkPanel;

    // --- 플레이어 무기 위젯 ---
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UPlayerWeaponWidget> PlayerWeaponPanel;
};
