// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GamePlayTagContainer.h"
#include "PerkDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class TEAMPOTATO_API UPerkDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FText PerkName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta =(MultiLine = true))
	FText Description;

	// this perks Effect class
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Logic")
	TObjectPtr<class UPerkEffectBase> PerkEffect;
};
