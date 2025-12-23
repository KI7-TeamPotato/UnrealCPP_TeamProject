// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/RangeSwordWeaponActor.h"
#include "Item/Weapon/SlashActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Player/TestCharacter.h"

void ARangeSwordWeaponActor::EndAttack()
{
    Super::EndAttack();

    if (!SlashActorClass)
        return;

    OwnerCharacter = Cast<ATestCharacter>(GetOwner());
    if (!OwnerCharacter)
        return;

    FVector Forward = OwnerCharacter->GetActorForwardVector();
    FVector SpawnLocation = OwnerCharacter->GetActorLocation() + Forward * 100.f + FVector(0, 0, 50.f);
    FRotator SpawnRotation = OwnerCharacter->GetActorRotation();

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = OwnerCharacter;
    SpawnParams.Instigator = OwnerCharacter->GetInstigator();
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    ASlashActor* Slash = GetWorld()->SpawnActor<ASlashActor>(
        SlashActorClass,
        SpawnLocation,
        SpawnRotation,
        SpawnParams
    );

    if (Slash)
    {
        // 바라보는 방향으로만 이동
        FVector ForwardDir = SpawnRotation.Vector();
        Slash->ProjectileMovementComponent->Velocity =
            ForwardDir * Slash->ProjectileMovementComponent->InitialSpeed;
    }
}
