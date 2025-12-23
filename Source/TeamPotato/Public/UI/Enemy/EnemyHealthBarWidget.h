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
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> EnemyHealthBar;
};
