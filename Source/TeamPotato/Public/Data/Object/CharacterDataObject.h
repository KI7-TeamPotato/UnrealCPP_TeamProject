// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/CharacterTypes.h"
#include "CharacterDataObject.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UCharacterDataObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ECharacterType CharacterType = ECharacterType::None;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UTexture2D> CharacterIcon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> CharacterIllustration = nullptr;

public:
	// --- 데이터 테이블 행을 받으면 해당 데이터행의 값들로 초기화 ---
	void InitFromDataTableRow(const struct FCharacterDataTableRow& DataRow);
};
