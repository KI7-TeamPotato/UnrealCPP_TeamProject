// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TestUIController.generated.h"

class UMVVMSubsystem;
class UPerkSelectionScreenWidget;
class UPerkDataAsset;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API ATestUIController : public APlayerController
{
	GENERATED_BODY()
	
protected:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "UI|MainHud")
    void InitializeViewModels(UMVVMSubsystem* Subsystem);

    UFUNCTION()
    void RemovePerkSelectionScreenFromViewport(UPerkDataAsset* EquippedPerk);

    UFUNCTION()
    void AddPerkSelectionScreenToViewport();

protected:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UPerkSelectionScreenWidget> PerkSelectionScreenClass;

    // --- 퍽 선택 위젯 ---
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UPerkSelectionScreenWidget> PerkSelectionScreen;
};
