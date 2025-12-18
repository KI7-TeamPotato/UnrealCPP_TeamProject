// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TestUIController.generated.h"

class UMVVMSubsystem;
class UPerkSelectionScreenWidget;
class UPerkDataAsset;
class UMinimapWidget;
class UMinimapViewModel;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API ATestUIController : public APlayerController
{
	GENERATED_BODY()
	
protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    UFUNCTION(BlueprintCallable, Category = "UI|MainHud")
    void InitializeViewModels(UMVVMSubsystem* Subsystem);

    UFUNCTION()
    void RemovePerkSelectionScreenFromViewport(UPerkDataAsset* EquippedPerk);

    UFUNCTION()
    void AddPerkSelectionScreenToViewport();

    void UpdateMinimapPlayerPosition();

protected:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UPerkSelectionScreenWidget> PerkSelectionScreenClass;

    // --- 퍽 선택 위젯 ---
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UPerkSelectionScreenWidget> PerkSelectionScreen;
   
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UMinimapWidget> MinimapWidgetClass;

    UPROPERTY()
    TObjectPtr<UMinimapWidget> MinimapWidget = nullptr;

private:
    TObjectPtr<UMinimapViewModel> MinimapViewModel = nullptr;

    FVector LastPawnLocation = FVector::ZeroVector;
    float LastPawnYaw = 0.f;

    // --- 거리 업데이트 임계값 ---
    UPROPERTY(EditDefaultsOnly, Category = "Minimap")
    float MinimapUpdateThreshold = 10.f;

    UPROPERTY(EditDefaultsOnly, Category = "Minimap")
    float MinimapYawUpdateThreshold = 5.f;

    FTimerHandle MinimapUpdateTimer;
};
