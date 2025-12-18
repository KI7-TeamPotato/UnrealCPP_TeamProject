// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/WeaponComponent.h"
#include "Player/TestCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Item/Weapon/GunWeaponActor.h"
#include "Data/WeaponDataAsset.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<ATestCharacter>(GetOwner());
}

void UWeaponComponent::WeaponAttack()
{
	if (!CurrentWeapon) return;

	if (CurrentWeapon)
	{
		CurrentWeapon->Attack(Owner);
	}
}

void UWeaponComponent::EquipWeapon(UWeaponDataAsset* WeaponData)
{
	if (!Owner || !WeaponData || !WeaponData->WeaponClass) return;

	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponData->WeaponClass);

	if (!CurrentWeapon) return;

    Owner->SetPlayerActivatedWeapon(CurrentWeapon->GetWeaponType());
	CurrentWeapon->SetOwner(Owner);
	CurrentWeapon->SetOwnerComponent(this);
    CurrentWeapon->InitializeFromData(WeaponData);

	CurrentWeapon->AttachToComponent(
		Owner->GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		TEXT("Weapon_R")
	);
}

EWeaponType UWeaponComponent::GetCurrentWeaponType() const
{
	return CurrentWeapon ? CurrentWeapon->GetWeaponType() : EWeaponType::None;
}
