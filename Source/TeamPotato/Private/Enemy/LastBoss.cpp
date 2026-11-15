// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/LastBoss.h"
#include "TeamPotato/Logic/DungeonGanarator.h"
#include "TeamPotato/Room/RoomBase.h"
#include "Kismet/GameplayStatics.h"
ALastBoss::ALastBoss()
{
    MaxHealth = 1200;
}

void ALastBoss::BeginPlay()
{
    Super::BeginPlay();
}

void ALastBoss::OnDie()
{
    if (bDeathHandled || !HasAuthority()) return;
    Super::OnDie();

    ARoomBase* Room = EncounterRoom.Get();
    auto* Dungeon = Room ? Cast<ADungeonGanarator>(Room->GetOwner()) : nullptr;
    if (Dungeon && Dungeon->HasNextChapter())
    {
        Dungeon->OpenNextChapterPortal(*Room);
        return;
    }

    constexpr float VictoryDelay = 14.0f;
    if (VictoryWidgetClass && GetLifeSpan() > 0.0f && GetLifeSpan() <= VictoryDelay)
        SetLifeSpan(VictoryDelay + 1.0f);
    GetWorldTimerManager().SetTimer(VictoryWidgetTimerHandle, this, &ALastBoss::ShowVictoryWidget, VictoryDelay, false);
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
