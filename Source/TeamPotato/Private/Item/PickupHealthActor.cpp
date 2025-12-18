// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupHealthActor.h"
#include "Player/TestCharacter.h"
#include "Component/PlayerResource.h"

void APickupHealthActor::UseItem(AActor* InPlayer)
{
    ATestCharacter* Player = Cast<ATestCharacter>(InPlayer);
    if (!Player) return;

    UPlayerResource* Resource = Player->GetResource();
    if (!Resource) return;

    Resource->Heal(HealAmount);
}