// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupGoldActor.h"
#include "Player/TestCharacter.h"
#include "Component/PlayerResource.h"

void APickupGoldActor::OnPickup(AActor* InPlayer)
{
    if (!InPlayer) return;

    AddGold(InPlayer);
    Destroy();
}

void APickupGoldActor::AddGold(AActor* InPlayer)
{
    ATestCharacter* Player = Cast<ATestCharacter>(InPlayer);
    if (!Player) return;

    UPlayerResource* Resource = Player->GetResource();
    if (!Resource) return;

    Resource->AddGold(GoldAmount);
}
