// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupStaminaActor.h"
#include "Player/TestCharacter.h"
#include "Component/PlayerResource.h"
#include "Kismet/GameplayStatics.h"

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

    if (PickupSound)
        UGameplayStatics::PlaySound2D(this, PickupSound);

    Resource->UseEnergy(-StaminaAmount);
}