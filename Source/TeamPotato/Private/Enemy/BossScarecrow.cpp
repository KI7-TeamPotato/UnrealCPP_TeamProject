// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BossScarecrow.h"
#include "TeamPotato/Room/DungeonRoom9.h"
#include "Kismet/GameplayStatics.h"
ABossScarecrow::ABossScarecrow()
{
    MaxHealth = 300;


}

void ABossScarecrow::BeginPlay()
{
    Super::BeginPlay();
}

void ABossScarecrow::OnDie()
{
    if (bDeathHandled || !HasAuthority()) return;
    Super::OnDie();

    ADungeonRoom9* Room = Cast<ADungeonRoom9>(EncounterRoom.Get());
    if (!EncounterRoom.IsValid())
        Room = Cast<ADungeonRoom9>(UGameplayStatics::GetActorOfClass(GetWorld(), ADungeonRoom9::StaticClass()));
    if (Room)
    {
        Room->SpawnPortal(); 
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CannotfindDungeonRoom9"));
    }
}

float ABossScarecrow::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
