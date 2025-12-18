// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/EnumBase.h"
#include "Item/Weapon/WeaponBase.h"
#include "Item/Weapon/WeaponPickupActor.h"
#include "WeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
    // 무기 타입
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    EWeaponType WeaponType = EWeaponType::None;

    // 장착 무기
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actor Classes")
    TSubclassOf<AWeaponBase> WeaponClass;

    // 드랍 무기
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actor Classes")
    TSubclassOf<AWeaponPickupActor> PickupWeaponClass;

    // 무기 데미지
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Data")
    float AttackDamage = 0.0f;

    // 무기 소모 에너지
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Data")
    float AttackCost = 0.0f;

    // 무기 가격
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Data")
    int32 WeaponPrice = 0;
};
