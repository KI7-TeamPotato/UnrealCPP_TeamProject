// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerResource.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, InCurrentHealth, float, InMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnResourceChanged, float, InCurrentEnergy, float, InMaxEnergy);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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
	bool UseStamina(float InUseStaminaAmount);


private:
    inline bool IsEnergyRemain(float InUseStaminaAmount) { return (Energy > InUseStaminaAmount); }

    //inline bool IsStaminaRemain(float InUseStaminaAmount) { return (Stamina > InUseStaminaAmount); }

    void BroadcastHealthChanged();
    void BroadcastEnergyChanged();

public:
    // --- 체력,자원 변경 델리게이트 ---
    UPROPERTY(BlueprintAssignable, Category = "Resource")
    FOnHealthChanged OnHealthChanged;
    UPROPERTY(BlueprintAssignable, Category = "Resource")
    FOnResourceChanged OnEnergyChanged;

private:
	//체력
	float Health = 100.0f;
	//최대 체력
	const float MaxHealth = 100.0f;
    //사망 확인 기준점. 체력이 이보다 작을 시 사망처리함
    const float MinHealth = 0.0001;

    //무기 공격에 사용되는 자원
    float Energy = 100.0f;
    //최대 에너지
    const float MaxEnergy = 100.0f;


    ////스태미나
    //float Stamina = 100.0f;
    ////최대 스태미나
    //const float MaxStamina = 100.0f;
 
 
	//공격력
	float AttackPower = 10.0f;
	//최소 공격력(이 이하로 내려가지 않음)
	float MinAttackPower = 0.1f;

	//살아있는지 죽었는지
	bool bIsAlive = true;
};
