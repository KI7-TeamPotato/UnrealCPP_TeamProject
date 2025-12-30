// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BossScarecrow.h"
#include "TeamPotato/Room/DungeonRoom9.h"
#include "Kismet/GameplayStatics.h"
ABossScarecrow::ABossScarecrow()
{
    MaxHealth = 300;
    CurrentHealth = MaxHealth;
}

void ABossScarecrow::BeginPlay()
{
    Super::BeginPlay();
}

void ABossScarecrow::OnDie()
{
    Super::OnDie();

    AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ADungeonRoom9::StaticClass());

    // 3. 찾았다면 캐스팅 후 함수 실행
    if (ADungeonRoom9* Room = Cast<ADungeonRoom9>(FoundActor))
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
