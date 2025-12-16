// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterTypes.generated.h"

/**
 * 
 */
// --- 캐릭터 타입 ---
UENUM(BlueprintType)
enum class ECharacterType :uint8
{
	None		UMETA(DisplayName = "None"),
	Warrior		UMETA(DisplayName = "Warrior"),
	Archer		UMETA(DisplayName = "Archer"),
	Gunner		UMETA(DisplayName = "Gunner"),
};
