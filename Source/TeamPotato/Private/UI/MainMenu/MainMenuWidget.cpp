// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenu/MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/MainMenu/MainMenuButtonWidget.h"
#include "Subsystem/GameStateSubsystem.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (NewGameButton)
	{
		NewGameButton->MainMenuButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnNewGameClicked);
	}
	//if (ContinueGameButton)
	//{
	//	ContinueGameButton->MainMenuButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnContinueGameClicked);
	//}
	if (SettingsButton)
	{
		SettingsButton->MainMenuButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnSettingClicked);
	}
	if (QuitButton)
	{
		QuitButton->MainMenuButton->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitClicked);
	}
}

void UMainMenuWidget::OnNewGameClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("New Game Clicked"));

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UGameStateSubsystem* GameStateSubsystem = GI->GetSubsystem<UGameStateSubsystem>())
		{
			GameStateSubsystem->TravelToLobby();
		}
	}
}

//void UMainMenuWidget::OnContinueGameClicked()
//{
//	UE_LOG(LogTemp, Warning, TEXT("Continue Game Clicked"));
//}

void UMainMenuWidget::OnSettingClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Settings Clicked"));
}

void UMainMenuWidget::OnQuitClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Quit Clicked"));

	APlayerController* PC = GetOwningPlayer();

	UKismetSystemLibrary::QuitGame(
		this,
		PC,
		EQuitPreference::Quit,
		false
	);
}
