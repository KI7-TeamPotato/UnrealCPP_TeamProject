// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GamePlayTagContainer.h"
#include "Data/EPerkRarity.h"
#include "Player/Perk/PerkEffectBase.h"
#include "PerkDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class TEAMPOTATO_API UPerkDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perk")
	FText PerkName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perk")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perk", meta =(MultiLine = true))
	FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perk")
    EPerkRarity PerkRarity = EPerkRarity::Common;

	// this perks Effect class
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Logic")
	TObjectPtr<class UPerkEffectBase> PerkEffect;

    // 퍽 설명에 PerkEffect의 값들을 적용하여 포맷팅된 설명을 반환합니다.
    // 서브시스템으로 빼면 더 좋을 수도? 다양한 효과가 들어오면 고려
    UFUNCTION(BlueprintCallable, Category = "Perk")
    FText GetFormattedDescription() const
    {
        FFormatNamedArguments Args;

        // 퍽의 효과 수치를 포맷팅 인수로 추가
        Args.Add(TEXT("Amount"), FText::AsNumber(PerkEffect->GetAmount()));
        return FText::Format(Description, Args);
    }
};
