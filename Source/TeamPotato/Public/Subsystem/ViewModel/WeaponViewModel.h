// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WeaponViewModel.generated.h"

// --- 위젯에 자원 변경을 알리기 위한 델리게이트 ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerResourceChanged, float, NewResourcePercent);
// --- 위젯에 메인 무기 변경을 알리기 위한 델리게이트 ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMainWeaponChanged, FText, InWeaponName, UTexture2D*, InWeaponIcon);
// --- 위젯에 서브 무기 변경을 알리기 위한 델리게이트 ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSubWeaponChanged, FText, InWeaponName, UTexture2D*, InWeaponIcon);


struct FWeaponInfo
{
    // --- 무기 이름 ---
    FText WeaponName;
    // --- 무기 아이콘 ---
    UTexture2D* WeaponIcon;
};
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UWeaponViewModel : public UObject
{
	GENERATED_BODY()
	
public:
    // --- 플레이어의 자원 설정 함수 ---
    UFUNCTION(BlueprintCallable)
    void SetResource(float CurrentResource, float MaxResource);

    UFUNCTION(BlueprintCallable)
    void SetMainWeapon(FText InWeaponName, UTexture2D* InWeaponIcon);

    UFUNCTION(BlueprintCallable)
    void SetSubWeapon(FText InWeaponName, UTexture2D* InWeaponIcon);

    UFUNCTION(BlueprintCallable)
    void SwapMainAndSubWeapon();

public:
    // --- 델리게이트 ---
    FOnPlayerResourceChanged OnPlayerResourceChanged;
    FOnMainWeaponChanged OnMainWeaponChanged;
    FOnSubWeaponChanged OnSubWeaponChanged;

private:
    // --- 플레이어의 자원 백분율 ---
    float ResourcePercent = 1.f;

    // --- 메인 무기 정보 ---
    FWeaponInfo MainWeapon;

    // --- 서브 무기 정보 ---
    FWeaponInfo SubWeapon;
};
