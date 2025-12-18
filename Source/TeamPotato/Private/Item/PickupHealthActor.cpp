// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupHealthActor.h"
#include "Player/TestCharacter.h"
#include "Component/PlayerResource.h"

void APickupHealthActor::OnPickup(AActor* InPlayer)
{
    if (!InPlayer) return;

    Heal(InPlayer);
    Destroy();
}

void APickupHealthActor::Heal(AActor* InPlayer)
{
    ATestCharacter* Player = Cast<ATestCharacter>(InPlayer);
    if (!Player) return;

    UPlayerResource* Resource = Player->GetResource();
    if (!Resource) return;

    Resource->Heal(HealAmount);
}
