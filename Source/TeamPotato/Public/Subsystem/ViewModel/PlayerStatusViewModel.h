// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/CharacterTypes.h"
#include "PlayerStatusViewModel.generated.h"

// --- 위젯에 체력 및 자원 변경을 알리기 위한 델리게이트 ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerHealthChangedCurrentMaxHealth, float, InCurrentHealth, float, InMaxHealth);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerResourceChanged, float, NewResourcePercent);

// --- 위젯에 플레이어 아이콘 변경을 알리기 위한 델리게이트 --- 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerIconChanged, UTexture2D*, NewPlayerIcon);

// --- 체력 텍스트 포함 버전 ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerHealthChanged, float, NewHealthPercent, FText, NewHealthText);

// --- 이동 속도 변경 델리게이트 ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerWalkSpeedChanged, float, NewWalkSpeed);
/**
 * 
 */
UCLASS(Blueprintable)
class TEAMPOTATO_API UPlayerStatusViewModel : public UObject
{
	GENERATED_BODY()
	
public:
	// --- Getters ---
	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const { return HealthPercent; }

	UFUNCTION(BlueprintPure)
	UTexture2D* GetPlayerIcon() const { return PlayerIcon; }

	// --- 플레이어의 체력 설정 함수 ---
	UFUNCTION(BlueprintCallable)
	void SetHealth(float CurrentHealth, float MaxHealth);

	// --- 플레이어 아이콘 설정 함수 ---
	UFUNCTION(BlueprintCallable)
	void SetPlayerIcon(UTexture2D* NewIcon);

    UFUNCTION(BlueprintCallable)
    void UpdateWalkSpeed(float NewWalkSpeed);

public:
	// --- 플레이어의 체력 변경 델리게이트 ---
	//UPROPERTY(BlueprintAssignable)
	//FOnPlayerHealthChanged OnPlayerHealthChanged;

	// --- 플레이어 아이콘 변경 델리게이트 ---
	UPROPERTY(BlueprintAssignable)
	FOnPlayerIconChanged OnPlayerIconChanged;

	// --- 체력 텍스트 포함 버전 ---
    UPROPERTY(BlueprintAssignable)
    FOnPlayerHealthChanged OnPlayerHealthChangedWithText;

    UPROPERTY(BlueprintAssignable)
    FOnPlayerHealthChangedCurrentMaxHealth OnPlayerHealthChangedCurrentMaxHealth;

    UPROPERTY(BlueprintAssignable)
    FOnPlayerWalkSpeedChanged OnPlayerWalkSpeedChanged;

private:
	// --- 플레이어의 체력 백분율 ---
	float HealthPercent=1.f;

	// --- 플레이어 아이콘 ---
	UTexture2D* PlayerIcon=nullptr;
};
