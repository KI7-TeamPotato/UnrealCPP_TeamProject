// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/CharacterTypes.h"
#include "PlayerSaveData.generated.h"

class UWeaponDataAsset;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FPlayerSaveData
{
    GENERATED_BODY()

public:
    // 선택된 캐릭터 타입
    UPROPERTY(BlueprintReadWrite, Category = "Player Save Data")
    ECharacterType SelectedCharacterType = ECharacterType::Warrior;
    // 장착 중인 메인 무기
    UPROPERTY(BlueprintReadWrite, Category = "Player Save Data")
    TObjectPtr<UWeaponDataAsset> EquippedMainWeapon = nullptr;
    // 장착 중인 서브 무기
    UPROPERTY(BlueprintReadWrite, Category = "Player Save Data")
    TObjectPtr<UWeaponDataAsset> EquippedSubWeapon = nullptr;
    // 플레이어 아이콘
    UPROPERTY(BlueprintReadWrite, Category = "Player Save Data")
    TObjectPtr<UTexture2D> PlayerIcon = nullptr;
    // 플레이어 일러스트
    UPROPERTY(BlueprintReadWrite, Category = "Player Save Data")
    TObjectPtr<UTexture2D> PlayerIllustration = nullptr;
    UPROPERTY(BlueprintReadWrite, Category = "Player Save Data")
    float MaxHealth = 100.0f;
    UPROPERTY(BlueprintReadWrite, Category = "Player Save Data")
    float MaxEnergy = 100.0f;
    UPROPERTY(BlueprintReadWrite, Category = "Player Save Data")
    float WalkSpeed = 600.0f;
};