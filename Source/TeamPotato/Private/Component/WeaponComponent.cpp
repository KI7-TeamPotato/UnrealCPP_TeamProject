// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/WeaponComponent.h"
#include "Player/TestCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Item/Weapon/GunWeaponActor.h"

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
	if (!Owner || !WeaponClass) return;

	CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass);
	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwner(Owner);
		CurrentWeapon->SetOwnerComponent(this);

		CurrentWeapon->AttachToComponent(
			Owner->GetMesh(),
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			TEXT("Weapon_R")
		);
	}
}

void UWeaponComponent::WeaponAttack()
{
	if (!CurrentWeapon) return;

	if (CurrentWeapon)
	{
		CurrentWeapon->Attack(Owner);
	}
}

void UWeaponComponent::EquipWeapon(TSubclassOf<AWeaponBase> InWeapon)
{
	if (!Owner || !InWeapon) return;

	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	CurrentWeapon = GetWorld()->SpawnActor<AWeaponBase>(InWeapon);
	if (!CurrentWeapon) return;
	CurrentWeapon->SetOwner(Owner);
	CurrentWeapon->SetOwnerComponent(this);

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
