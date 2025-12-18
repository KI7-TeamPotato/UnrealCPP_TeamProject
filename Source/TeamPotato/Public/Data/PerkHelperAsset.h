// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/EPerkRarity.h"
#include "PerkHelperAsset.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UPerkHelperAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI|Perk")
    TMap<EPerkRarity, UTexture2D*> PerkFrame;
	

    UTexture2D* GetPerkFrameByRarity(EPerkRarity Rarity)
    {
        if (PerkFrame.Contains(Rarity))
        {
            return PerkFrame[Rarity];
        }
        return nullptr;
    }
};
