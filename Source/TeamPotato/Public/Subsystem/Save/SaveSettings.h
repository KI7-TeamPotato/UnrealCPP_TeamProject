// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveSettings.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API USaveSettings : public USaveGame
{
	GENERATED_BODY()
	
public:
    USaveSettings();

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Sound")
    float MusicVolume = 1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Settings|Sound")
    float SFXVolume = 1.0f;
};
