// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PlayerResource.h"
#include "Player/TestCharacter.h"
#include "Subsystem/MVVMSubsystem.h"
#include "Kismet/GameplayStatics.h"

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
	
    if (UMVVMSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UMVVMSubsystem>())
    {
        Subsystem->RegisterPlayerResourceComp(this);
    }

    Health = MaxHealth;
	Energy = MaxEnergy;
    Gold = 0;

    // 초기 체력, 에너지 값, 골드 브로드캐스트
    BroadcastHealthChanged();
    BroadcastEnergyChanged();
    BroadcastGoldChanged();
}

void UPlayerResource::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UMVVMSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UMVVMSubsystem>())
    {
        Subsystem->UnregisterPlayerResourceComp(this);
    }
    Super::EndPlay(EndPlayReason);
}

void UPlayerResource::PlayerTakeDamage(float InDamage)
{
	Health -= InDamage;

    // 체력 변경 브로드캐스트 시도
    BroadcastHealthChanged();

    UE_LOG(LogTemp, Log, TEXT("Left Health: %f"), Health);
	if (Health <= HealthEpsilon)
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
    // 체력 변경 브로드캐스트 시도
    BroadcastHealthChanged();
}

bool UPlayerResource::UseEnergy(float InUseStaminaAmount)
{
    if (IsEnergyRemain(InUseStaminaAmount))
    {
        Energy -= InUseStaminaAmount;
        
        UE_LOG(LogTemp, Log, TEXT("Stamina : %f"), Energy);
        // 에너지 변경 브로드캐스트 시도
        BroadcastEnergyChanged();
        return true;
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("No Stamina"));
        return false;
    }

	/*if (IsStaminaRemain(InUseStaminaAmount))
	{
		Stamina -= InUseStaminaAmount;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("No Stamina"));
		return false;
	}*/
}

void UPlayerResource::AddGold(int32 InGold)
{
    Gold += InGold;
}

// 최대 체력이나 현재 체력이 바뀌었을 때 뒤에 넣어서 브로드캐스트 해주는 함수
void UPlayerResource::BroadcastHealthChanged()
{
    if (OnHealthChanged.IsBound())
    {
        OnHealthChanged.Broadcast(Health, MaxHealth);
    }
}

void UPlayerResource::BroadcastEnergyChanged()
{
    if (OnEnergyChanged.IsBound())
    {
        OnEnergyChanged.Broadcast(Energy, MaxEnergy);
    }

}

void UPlayerResource::FillEnergy(float InEnergy)
{
    Energy += InEnergy;
    if (Energy > MaxEnergy)
    {
        Energy = MaxEnergy;
    }
    UE_LOG(LogTemp, Log, TEXT("Stamina : %f"), Energy);
    BroadcastEnergyChanged();
}

void UPlayerResource::AddPower(float InPower)
{
    AttackPower += InPower;
    UE_LOG(LogTemp, Log, TEXT("Power : %f"), AttackPower);
}

void UPlayerResource::AddMaxHealth(float InMaxHealth)
{
    MaxHealth += InMaxHealth;
    Heal(InMaxHealth);
}

void UPlayerResource::AddMaxEnergy(float InMaxStamina)
{
    MaxEnergy += InMaxStamina;
    FillEnergy(InMaxStamina);
}

void UPlayerResource::BroadcastGoldChanged()
{
    if (OnGoldChanged.IsBound())
    {
        //OnGoldChanged.Broadcast();
    }
}
