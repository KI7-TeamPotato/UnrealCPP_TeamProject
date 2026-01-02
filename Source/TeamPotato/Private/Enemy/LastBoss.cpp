// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/LastBoss.h"
#include "TeamPotato/Room/DungeonRoom9.h"
#include "Kismet/GameplayStatics.h"
ALastBoss::ALastBoss()
{
    MaxHealth = 1200;
    CurrentHealth = MaxHealth;
}

void ALastBoss::BeginPlay()
{
    Super::BeginPlay();
}

void ALastBoss::OnDie()
{
    Super::OnDie();

    GetWorldTimerManager().SetTimer(VictoryWidgetTimerHandle, this, &ALastBoss::ShowVictoryWidget, 14.0f, false);
}

float ALastBoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

}

void ALastBoss::ShowVictoryWidget()
{
    if (VictoryWidgetClass)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC)
        {
            VictoryWidget = CreateWidget<UUserWidget>(PC, VictoryWidgetClass);
            if (VictoryWidget)
            {
                VictoryWidget->AddToViewport();

                // 마우스 커서 및 입력 모드 설정
                PC->SetShowMouseCursor(true);
                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(VictoryWidget->TakeWidget());
                PC->SetInputMode(InputMode);
            }
        }
    }
}
