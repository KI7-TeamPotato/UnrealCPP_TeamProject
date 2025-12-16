// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PlayerResource.h"

// Sets default values for this component's properties
UPlayerResource::UPlayerResource()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UPlayerResource::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Health = MaxHealth;
	Stamina = MaxStamina;
}

void UPlayerResource::TakeDamage(float InDamage)
{
	Health -= InDamage;
	if (Health <= 0)
	{
		bIsAlive = false;
	}
}

void UPlayerResource::Heal(float InHeal)
{
	Health += InHeal;
	if (Health >= MaxHealth)
	{
		Health = MaxHealth;
	}
}

void UPlayerResource::UseStamina(float InUseStaminaAmount)
{
	Stamina -= InUseStaminaAmount;
}
