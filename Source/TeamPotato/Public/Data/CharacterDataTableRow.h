// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/CharacterTypes.h"
#include "Data/WeaponDataAsset.h"
#include "CharacterDataTableRow.generated.h"

class ATestCharacter;
/**
 * 
 */

USTRUCT(BlueprintType)
struct FCharacterDataTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
    ECharacterType CharacterType = ECharacterType::None;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
    FString CharacterName = "";

    UPROPERTY(EditAnyWhere, BlueprintReadOnly)
    TSubclassOf<ATestCharacter> CharacterClass = nullptr;

    UPROPERTY(EditAnyWhere, BlueprintReadOnly)
    TObjectPtr<UWeaponDataAsset> DefaultWeaponDataAsset = nullptr;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
    TObjectPtr<UTexture2D> CharacterIcon = nullptr;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
    TObjectPtr<UTexture2D> CharacterIllustration = nullptr;
};
