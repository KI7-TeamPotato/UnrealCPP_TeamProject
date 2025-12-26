// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyWeaponBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"

AEnemyWeaponBase::AEnemyWeaponBase()
{
    PrimaryActorTick.bCanEverTick = false;

    WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
    RootComponent = WeaponMesh;

    CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule"));
    CollisionCapsule->SetupAttachment(WeaponMesh);

    CollisionCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));

    CollisionCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionCapsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AEnemyWeaponBase::EnableAttackCollision()
{
    // 이미지의 'Set Collision Enabled (Query Only)'와 동일
    if (CollisionCapsule)
    {
        CollisionCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }
}

void AEnemyWeaponBase::DisableAttackCollision()
{
    // 이미지의 'Set Collision Enabled (No Collision)'과 동일
    if (CollisionCapsule)
    {
        CollisionCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}