// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerResource.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, InCurrentHealth, float, InMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnResourceChanged, float, InCurrentEnergy, float, InMaxEnergy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoldChanged, int32, InCurrentGold);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEAMPOTATO_API UPlayerResource : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UPlayerResource();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    //getter
    inline float GetHealthAmount() { return Health; }
    inline float GetEnergyAmount() { return Energy; }

    //inline float GetStaminaAmount() { return Stamina; }

    //리소스 사용
    UFUNCTION(BlueprintCallable, Category = "Resource")
    void PlayerTakeDamage(float InDamage);
    UFUNCTION(BlueprintCallable, Category = "Resource")
    void Heal(float InHeal);
    UFUNCTION(BlueprintCallable, Category = "Resource")
    bool UseEnergy(float InUseStaminaAmount);
    UFUNCTION(BlueprintCallable, Category = "Resource")
    void FillEnergy(float InStamina);
    UFUNCTION(BlueprintCallable, Category = "Resource")
    void AddGold(int32 InGold);

    //스탯 변화
    void AddPower(float InPower);
    void AddMaxHealth(float InMaxHealth);
    void AddMaxEnergy(float InMaxStamina);

private:
    //스태미나가 사용할만큼 충분히 있는지 확인
    inline bool IsEnergyRemain(float InUseEnergyAmount) { return (Energy > (InUseEnergyAmount - EnergyEpsilon)); }

    void BroadcastHealthChanged();
    void BroadcastEnergyChanged();
    void BroadcastGoldChanged();

public:
    // --- 체력,자원 변경 델리게이트 ---
    UPROPERTY(BlueprintAssignable, Category = "Resource")
    FOnHealthChanged OnHealthChanged;
    UPROPERTY(BlueprintAssignable, Category = "Resource")
    FOnResourceChanged OnEnergyChanged;
    UPROPERTY(BlueprintAssignable, Category = "Resource")
    FOnGoldChanged OnGoldChanged;

private:
    //체력
    float Health = 100.0f;
    //최대 체력
    float MaxHealth = 100.0f;
    //최대 체력의 최솟값. 최대체력 감소 효과 적용시 이 이하로 내려가지 않음
    const float MinHealth = 1.0f;
    //사망 확인 기준점. 체력이 이보다 작을 시 사망처리함
    const float HealthEpsilon = 0.0001;

    //스태미나
    float Energy = 100.0f;
    //최대 스태미나
    float MaxEnergy = 100.0f;
    //최대 스태미나의 최솟값. 최대 스태미나 감소 효과 적용시 이 이하로 내려가지 않음
    const float MinEnergy = 10.0f;
    //스태미나가 충분히 있는지 계산할때 부동소수점 오차로 인해 사용
    const float EnergyEpsilon = 0.0001f;
    // 소지 골드
    int32 Gold = 0;

    //공격력
    float AttackPower = 10.0f;
    //최소 공격력(이 이하로 내려가지 않음)
    float MinAttackPower = 0.1f;

    //살아있는지 죽었는지
    bool bIsAlive = true;
};
