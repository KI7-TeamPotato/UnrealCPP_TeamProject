// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/RangeSwordWeaponActor.h"
#include "Item/Weapon/SlashActor.h"
#include "GameFramework/ProjectileMovementComponent.h"

void ARangeSwordWeaponActor::EndAttack()
{
    Super::EndAttack();

    if (!SlashActorClass)
        return;

    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
        return;

    FVector Forward = OwnerActor->GetActorForwardVector();
    FVector SpawnLocation = OwnerActor->GetActorLocation() + Forward * 100.f + FVector(0, 0, 50.f);
    FRotator SpawnRotation = OwnerActor->GetActorRotation();

    FActorSpawnParameters Params;
    Params.Owner = OwnerActor;
    Params.Instigator = OwnerActor->GetInstigator();

    ASlashActor* Slash = GetWorld()->SpawnActor<ASlashActor>(
        SlashActorClass,
        SpawnLocation,
        SpawnRotation,
        Params
    );

    if (Slash)
    {
        // 바라보는 방향으로만 이동
        FVector ForwardDir = SpawnRotation.Vector();
        Slash->ProjectileMovementComponent->Velocity =
            ForwardDir * Slash->ProjectileMovementComponent->InitialSpeed;
    }
}
