// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupStaminaActor.h"
#include "Player/TestCharacter.h"
#include "Component/PlayerResource.h"

APickupStaminaActor::APickupStaminaActor()
{
    ItemPrice = 10;
}

void APickupStaminaActor::UseItem(AActor* InPlayer)
{
    ATestCharacter* Player = Cast<ATestCharacter>(InPlayer);
    if (!Player) return;

    UPlayerResource* Resource = Player->GetResource();
    if (!Resource) return;

    Resource->UseEnergy(-StaminaAmount);
}