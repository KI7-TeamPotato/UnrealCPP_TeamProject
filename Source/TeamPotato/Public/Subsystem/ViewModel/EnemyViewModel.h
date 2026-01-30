// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EnemyViewModel.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUpdateBossHealth, float, InCurrentHealth, float, InMaxHealth);

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UEnemyViewModel : public UObject
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable)
    void SetBossHealth(float CurrentHealth, float MaxHealth);

public:
    UPROPERTY(BlueprintAssignable)
    FUpdateBossHealth UpdateBossHealth;
};
