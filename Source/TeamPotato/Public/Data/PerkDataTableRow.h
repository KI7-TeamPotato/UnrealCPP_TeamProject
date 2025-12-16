// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/PerkDataAsset.h"
#include "PerkDataTableRow.generated.h"

/**
 * 
 */
//
// 뽑기를 위한 구조체 Efraimidis-Spirakis 알고르즘
//
struct FWeightCandidate
{
public:
	FPerkSelectDataTableRow* PerkRow;
	double Score;

	bool operator<(const FWeightCandidate& Other) const
	{
		return Score > Other.Score;
	}
};


USTRUCT(BlueprintType)
struct FPerkSelectDataTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	float SelectWeight;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	TObjectPtr<UPerkDataAsset> PerkDataAsset;
};