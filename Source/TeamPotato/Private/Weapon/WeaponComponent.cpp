// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/GunWeaponActor.h"

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

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter || !GunClass) return;

	CurrentGun = GetWorld()->SpawnActor<AGunWeaponActor>(GunClass);
	if (CurrentGun)
	{
		CurrentGun->SetOwner(OwnerCharacter);
		CurrentGun->SetOwnerComponent(this);

		CurrentGun->AttachToComponent(
			OwnerCharacter->GetMesh(),
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			TEXT("Weapon_R")
		);
	}
}

void UWeaponComponent::FireWeapon()
{
	if (CurrentGun)
	{
		CurrentGun->Fire();
	}
}
