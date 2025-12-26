// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/WeaponBase.h"
#include "Player/TestCharacter.h"
#include "Component/PlayerResource.h"
#include "Data/WeaponDataAsset.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    // 매쉬 초기화
    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    WeaponMesh->SetupAttachment(Root);
    WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
    Super::BeginPlay();

    if(EquipSound)
        UGameplayStatics::PlaySound2D(this, EquipSound);
}

void::AWeaponBase::Attack(class ATestCharacter* OwningPlayer)
{
    if (AttackSound)
        UGameplayStatics::PlaySound2D(this, AttackSound);
}

void AWeaponBase::InitializeFromData(UWeaponDataAsset* InData)
{
    if (!InData) return;

    WeaponData = InData;
    WeaponType = InData->WeaponType;
    AttackDamage = InData->AttackDamage;
    AttackCost = InData->AttackCost;
}