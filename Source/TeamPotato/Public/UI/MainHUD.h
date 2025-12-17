// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainHUD.generated.h"

class UMainHUDWidget;
class UMVVMSubsystem;

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API AMainHUD : public AHUD
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UMainHUDWidget> MainHUDWidgetClass;

    UPROPERTY()
    TObjectPtr<UMainHUDWidget> MainHUDWidget;
};