// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupHealthActor.h"
#include "Player/TestCharacter.h"
#include "Component/PlayerResource.h"
#include "Kismet/GameplayStatics.h"

APickupHealthActor::APickupHealthActor()
{
    ItemPrice = 20;
}

void APickupHealthActor::UseItem(AActor* InPlayer)
{
    ATestCharacter* Player = Cast<ATestCharacter>(InPlayer);
    if (!Player) return;

    UPlayerResource* Resource = Player->GetResource();
    if (!Resource) return;

    if (PickupSound)
        UGameplayStatics::PlaySound2D(this, PickupSound);

    Resource->Heal(HealAmount);
}