// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UMainMenuButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UMainMenuButtonWidget(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UButton> MainMenuButton;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<class UTextBlock> MainMenuButtonText;
};
