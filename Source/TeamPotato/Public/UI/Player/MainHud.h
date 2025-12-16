// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHud.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UMainHud : public UUserWidget
{
	GENERATED_BODY()
	
public:
    //UFUNCTION(BlueprintCallable, Category = "UI|MainHud")
    //void ShowPerkSelectionScreen();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	// --- 플레이어 스탯 위젯 ---
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UPlayerStatWidget> PlayerStatPanel;
};
