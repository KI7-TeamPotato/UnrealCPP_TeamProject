// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyDamagePopupWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UEnemyDamagePopupWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> DamageText;
};
