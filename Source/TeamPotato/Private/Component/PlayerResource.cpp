// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PlayerResource.h"
#include "Player/TestCharacter.h"
#include "Subsystem/MVVMSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "Combat/CombatAbilitySystemComponent.h"
#include "Combat/CombatAttributeSet.h"
#include "Combat/CombatFunctionLibrary.h"

// Sets default values for this component's properties
UPlayerResource::UPlayerResource()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = false;
    bWantsInitializeComponent = true;
}

void UPlayerResource::InitializeComponent()
{
    Super::InitializeComponent();
    EnsureCombatConnection();
}

void UPlayerResource::EnsureCombatConnection()
{
    UCombatAbilitySystemComponent* CurrentASC = GetOwner()
        ? GetOwner()->FindComponentByClass<UCombatAbilitySystemComponent>() : nullptr;
    if (CurrentASC != CombatAbilitySystem)
    {
        DisconnectCombatDelegates();
        CombatAbilitySystem = CurrentASC;
        if (CombatAbilitySystem)
        {
            CombatAbilitySystem->OnCombatHealthChanged.AddUObject(this, &ThisClass::HandleCombatHealthChanged);
            CombatAbilitySystem->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMaxEnergyBonusAttribute())
                .AddUObject(this, &ThisClass::HandleMaxEnergyBonusChanged);
        }
    }
    RefreshMaxEnergy();
}

void UPlayerResource::DisconnectCombatDelegates()
{
    if (IsValid(CombatAbilitySystem))
    {
        CombatAbilitySystem->OnCombatHealthChanged.RemoveAll(this);
        CombatAbilitySystem->GetGameplayAttributeValueChangeDelegate(UCombatAttributeSet::GetMaxEnergyBonusAttribute()).RemoveAll(this);
    }
    CombatAbilitySystem = nullptr;
}

// Called when the game starts
void UPlayerResource::BeginPlay()
{
    Super::BeginPlay();

    EnsureCombatConnection();

    if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
    {
        if (UMVVMSubsystem* Subsystem = GameInstance->GetSubsystem<UMVVMSubsystem>())
            Subsystem->RegisterPlayerResourceComp(this);
    }

	Energy = MaxEnergy;
    CurrentGold = 0;
    GoldRemainder = 0.0;

    // 초기 체력, 에너지 값, 골드 브로드캐스트
    BroadcastHealthChanged();
    BroadcastEnergyChanged();
    BroadcastGoldChanged();
}

void UPlayerResource::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    DisconnectCombatDelegates();
    if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
    {
        if (UMVVMSubsystem* Subsystem = GameInstance->GetSubsystem<UMVVMSubsystem>())
            Subsystem->UnregisterPlayerResourceComp(this);
    }
    Super::EndPlay(EndPlayReason);
}

float UPlayerResource::GetHealthAmount()
{
    EnsureCombatConnection();
    return CombatAbilitySystem ? CombatAbilitySystem->GetHealth() : 0.0f;
}

float UPlayerResource::GetEnergyAmount()
{
    EnsureCombatConnection();
    return Energy;
}

float UPlayerResource::GetMaxEnergyAmount()
{
    EnsureCombatConnection();
    return MaxEnergy;
}

void UPlayerResource::SetMaxHealth(float InMaxHealth)
{
    EnsureCombatConnection();
    if (CombatAbilitySystem) CombatAbilitySystem->SetMaxHealthAndFill(InMaxHealth);
}

void UPlayerResource::SetMaxEnergy(float InMaxEnergy)
{
    if (!FMath::IsFinite(InMaxEnergy)) return;
    EnsureCombatConnection();
    BaseMaxEnergy = FMath::Max(0.0f, InMaxEnergy);
    MaxEnergy = CalculateMaxEnergy();
    Energy = MaxEnergy;
    BroadcastEnergyChanged();
}

void UPlayerResource::PlayerTakeDamage(float InDamage)
{
    UCombatFunctionLibrary::ApplyCombatDamage(GetOwner(), InDamage, nullptr);
}

void UPlayerResource::Heal(float InHeal)
{
    EnsureCombatConnection();
    if (CombatAbilitySystem) CombatAbilitySystem->Heal(InHeal);
}

bool UPlayerResource::UseEnergy(float InUseStaminaAmount)
{
    if (!FMath::IsFinite(InUseStaminaAmount)) return false;
    EnsureCombatConnection();
    if (IsEnergyRemain(InUseStaminaAmount))
    {
        Energy = FMath::Clamp(Energy - InUseStaminaAmount, 0.0f, MaxEnergy);
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
    EnsureCombatConnection();
    if (InGold > 0)
    {
        float Multiplier = CombatAbilitySystem && CombatAbilitySystem->IsCombatInitialized()
            ? CombatAbilitySystem->GetNumericAttribute(UCombatAttributeSet::GetGoldGainMultiplierAttribute()) : 1.0f;
        if (!FMath::IsFinite(Multiplier)) Multiplier = 1.0f;
        // GAS attributes are floats. Normalize the authored rate before multiplying large rewards.
        const double RewardMultiplier = FMath::RoundToDouble(static_cast<double>(FMath::Max(0.0f, Multiplier)) * 1000000.0) / 1000000.0;
        const double Reward = static_cast<double>(InGold) * RewardMultiplier + GoldRemainder;
        const double Space = static_cast<double>(MAX_int32) - CurrentGold;
        if (Reward >= Space)
        {
            CurrentGold = MAX_int32;
            GoldRemainder = 0.0;
        }
        else
        {
            // Keep the thirteenth gold after ten one-gold pickups despite fractional arithmetic noise.
            const double WholeGold = FMath::FloorToDouble(Reward + 0.000001);
            CurrentGold += static_cast<int32>(WholeGold);
            GoldRemainder = FMath::Max(0.0, Reward - WholeGold);
        }
    }
    else if (InGold < 0)
    {
        CurrentGold = static_cast<int32>(FMath::Max<int64>(0, static_cast<int64>(CurrentGold) + InGold));
    }
    BroadcastGoldChanged();
}

void UPlayerResource::FillEnergy(float InEnergy)
{
    if (!FMath::IsFinite(InEnergy)) return;
    EnsureCombatConnection();
    Energy = FMath::Clamp(Energy + InEnergy, 0.0f, MaxEnergy);
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
    EnsureCombatConnection();
    if (CombatAbilitySystem) CombatAbilitySystem->AddMaxHealth(InMaxHealth);
}

void UPlayerResource::AddMaxEnergy(float InMaxStamina)
{
    if (!FMath::IsFinite(InMaxStamina)) return;
    EnsureCombatConnection();
    BaseMaxEnergy = FMath::Max(0.0f, BaseMaxEnergy + InMaxStamina);
    RefreshMaxEnergy();
}

float UPlayerResource::CalculateMaxEnergy() const
{
    const float Bonus = CombatAbilitySystem && CombatAbilitySystem->IsCombatInitialized()
        ? CombatAbilitySystem->GetNumericAttribute(UCombatAttributeSet::GetMaxEnergyBonusAttribute()) : 0.0f;
    return FMath::Max(0.0f, BaseMaxEnergy + (FMath::IsFinite(Bonus) ? Bonus : 0.0f));
}

void UPlayerResource::RefreshMaxEnergy()
{
    const float NewMax = CalculateMaxEnergy();
    if (NewMax == MaxEnergy) return;
    const float Increase = FMath::Max(0.0f, NewMax - MaxEnergy);
    MaxEnergy = NewMax;
    Energy = FMath::Clamp(Energy + Increase, 0.0f, MaxEnergy);
    BroadcastEnergyChanged();
}

void UPlayerResource::HandleMaxEnergyBonusChanged(const FOnAttributeChangeData& Data)
{
    RefreshMaxEnergy();
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
