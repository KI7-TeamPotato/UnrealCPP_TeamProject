// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthBarWidget.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UEnemyHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    void SetHealthPercent(float Percent);

private:
    UFUNCTION()
    void UpdateDelayBar();

    UFUNCTION()
    void StartDelayBarTimer();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DelayBar")
    float InterpDuration = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DelayBar")
    float DelayBarWaitTime = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DelayBar")
    float InterpAlpha = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DelayBar")
    float DelayBarInterval = 0.02f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemyWidget")
    TObjectPtr<UCurveFloat> DelayBarCurve;

private:
    float CurrentHealthPercent = 1.0f;
    float DelayBarPercent = 1.0f;

    FTimerHandle DelayBarTimerHandle;
    FTimerHandle DelayBarWaitTimerHandle;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> EnemyHealthBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> DelayProgressBar;
};
