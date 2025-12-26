// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/CharacterDataTableRow.h"
#include "CharacterSelectDetailWidget.generated.h"

class UTextBlock;
class UImage;
/**
 * 
 */

/// <summary>
///  CharacterSelectMenu에서 타일뷰의 OnItemClicked에서 선택한 캐릭터의 타입이 오면 서브시스템의 캐싱된 데이터를 활용하여 정보를 가져와서 표시
/// </summary>
UCLASS()
class TEAMPOTATO_API UCharacterSelectDetailWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void UpdateCharacterDetails(ECharacterType InCharacterType);

protected:
    // --- 캐릭터 이름 ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> CharacterTypeText = nullptr;

    // --- 캐릭터 체력 ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> CharacterHealthText = nullptr;

    // --- 캐릭터 에너지 ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> CharacterEnergyText = nullptr;

    // --- 캐릭터 이동 속도 ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> CharacterWalkSpeedText = nullptr;

    // --- 무기 아이콘 ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> WeaponIconImage = nullptr;

    // --- 무기 이름 ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> WeaponNameText = nullptr;

    // --- 무기 데미지 ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> WeaponDamageText = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UTextBlock> WeaponAttackSpeedText = nullptr;

    // --- 캐릭터 일러스트 ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<UImage> CharacterIllustImage = nullptr;
};
