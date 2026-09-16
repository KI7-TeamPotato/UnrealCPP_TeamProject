// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PlayerResource.h"
#include "Player/TestCharacter.h"
#include "Subsystem/MVVMSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Combat/CombatAbilitySystemComponent.h"
#include "Combat/CombatFunctionLibrary.h"

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

    CombatAbilitySystem = GetOwner()->FindComponentByClass<UCombatAbilitySystemComponent>();
    if (CombatAbilitySystem)
    {
        CombatAbilitySystem->OnCombatHealthChanged.AddUObject(this, &ThisClass::HandleCombatHealthChanged);
    }

    // ...

    if (UMVVMSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UMVVMSubsystem>())
    {
        Subsystem->RegisterPlayerResourceComp(this);
    }

	Energy = MaxEnergy;
    CurrentGold = 0;

    // 초기 체력, 에너지 값, 골드 브로드캐스트
    BroadcastHealthChanged();
    BroadcastEnergyChanged();
    BroadcastGoldChanged();
}

void UPlayerResource::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (CombatAbilitySystem) CombatAbilitySystem->OnCombatHealthChanged.RemoveAll(this);
    if (UMVVMSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UMVVMSubsystem>())
    {
        Subsystem->UnregisterPlayerResourceComp(this);
    }
    Super::EndPlay(EndPlayReason);
}

float UPlayerResource::GetHealthAmount()
{
    return CombatAbilitySystem ? CombatAbilitySystem->GetHealth() : 0.0f;
}

void UPlayerResource::SetMaxHealth(float InMaxHealth)
{
    if (CombatAbilitySystem) CombatAbilitySystem->SetMaxHealthAndFill(InMaxHealth);
}

void UPlayerResource::SetMaxEnergy(float InMaxEnergy)
{
    MaxEnergy = InMaxEnergy;
    Energy = MaxEnergy;
    BroadcastEnergyChanged();
}

void UPlayerResource::PlayerTakeDamage(float InDamage)
{
    UCombatFunctionLibrary::ApplyCombatDamage(GetOwner(), InDamage, nullptr);
}

void UPlayerResource::Heal(float InHeal)
{
    if (CombatAbilitySystem) CombatAbilitySystem->Heal(InHeal);
}

bool UPlayerResource::UseEnergy(float InUseStaminaAmount)
{
    
    if (IsEnergyRemain(InUseStaminaAmount))
    {
        Energy -= InUseStaminaAmount;
        if (Energy > MaxEnergy)
        {
            Energy = MaxEnergy;
        }
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
}

void UPlayerResource::AddGold(int32 InGold)
{
    CurrentGold += InGold;
    BroadcastGoldChanged();
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
    if (CombatAbilitySystem) CombatAbilitySystem->AddMaxHealth(InMaxHealth);
}

void UPlayerResource::AddMaxEnergy(float InMaxStamina)
{
    MaxEnergy += InMaxStamina;
    FillEnergy(InMaxStamina);
    BroadcastEnergyChanged();
}

// 최대 체력이나 현재 체력이 바뀌었을 때 뒤에 넣어서 브로드캐스트 해주는 함수
void UPlayerResource::BroadcastHealthChanged()
{
    if (CombatAbilitySystem)
    {
        OnHealthChanged.Broadcast(CombatAbilitySystem->GetHealth(), CombatAbilitySystem->GetMaxHealth());
    }
}

void UPlayerResource::HandleCombatHealthChanged(float NewHealth, float NewMaxHealth)
{
    OnHealthChanged.Broadcast(NewHealth, NewMaxHealth);
}

void UPlayerResource::BroadcastEnergyChanged()
{
    if (OnEnergyChanged.IsBound())
    {
        OnEnergyChanged.Broadcast(Energy, MaxEnergy);
    }

}

void UPlayerResource::BroadcastGoldChanged()
{
    if (OnGoldChanged.IsBound())
    {
        OnGoldChanged.Broadcast(CurrentGold);
    }
}
