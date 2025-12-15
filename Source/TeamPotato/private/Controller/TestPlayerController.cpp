// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/TestPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubSystems.h"
#include "InputMappingContext.h"

void ATestPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* Subsystem =	ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (Subsystem)
	{
		UE_LOG(LogTemp, Log, TEXT("Player Controller Subsystem Available"));
		Subsystem->AddMappingContext(DefaultMappingContext, priority);
	}
}

void ATestPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}
