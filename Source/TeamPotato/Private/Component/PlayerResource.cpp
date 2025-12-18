// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PlayerResource.h"
#include "Player/TestCharacter.h"

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

void UPlayerResource::PlayerTakeDamage(float InDamage)
{
	Health -= InDamage;
    UE_LOG(LogTemp, Log, TEXT("Left Health: %f"), Health);
	if (Health <= MinHealth)
	{
		AActor* OwnerCharacter = GetOwner();
		Cast<ATestCharacter>(OwnerCharacter)->KillPlayer();
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

bool UPlayerResource::UseStamina(float InUseStaminaAmount)
{
    bool bIsStaminaEnough = IsStaminaRemain(InUseStaminaAmount);
	if (bIsStaminaEnough)
	{
		Stamina -= InUseStaminaAmount;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("No Stamina"));
	}
    UE_LOG(LogTemp, Log, TEXT("Stamina : %f"), Stamina);
    return bIsStaminaEnough;
}

void UPlayerResource::FillStamina(float InStamina)
{
    Stamina += InStamina;
    if (Stamina > MaxStamina)
    {
        Stamina = MaxStamina;
    }
    UE_LOG(LogTemp, Log, TEXT("Stamina : %f"), Stamina);
}