// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/PerkDataAsset.h"
#include "PerkDataObject.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UPerkDataObject : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UTexture2D* PerkIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (MultiLine = true))
	FText PerkDescription;

public:
	// --- 데이터 테이블 행을 받으면 해당 데이터행의 값들로 초기화 ---
	void InitFromDataTableAsset(UPerkDataAsset* PerDataAsset);
};
