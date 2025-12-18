// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerResource.generated.h"


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

public:
	//getter
	inline float GetHealthAmount() { return Health; }
	inline float GetStaminaAmount() { return Stamina; }

	//리소스 사용
	void PlayerTakeDamage(float InDamage);
	void Heal(float InHeal);
	bool UseStamina(float InUseStaminaAmount);
    void FillStamina(float InStamina);

    //스탯 변화
    void AddPower(float InPower);
    void AddMaxHealth(float InMaxHealth);
    void AddMaxStamina(float InMaxStamina);

private:
    //스태미나가 사용할만큼 충분히 있는지 확인
	inline bool IsStaminaRemain(float InUseStaminaAmount) { return (Stamina > (InUseStaminaAmount - StaminaEpsilon)); }

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
	float Stamina = 100.0f;
	//최대 스태미나
	float MaxStamina = 100.0f;
    //최대 스태미나의 최솟값. 최대 스태미나 감소 효과 적용시 이 이하로 내려가지 않음
    const float MinStamina = 10.0f;
    //스태미나가 충분히 있는지 계산할때 부동소수점 오차로 인해 사용
    const float StaminaEpsilon = 0.0001f;

	//공격력
	float AttackPower = 10.0f;
	//최소 공격력(이 이하로 내려가지 않음)
	float MinAttackPower = 0.1f;

	//살아있는지 죽었는지
	bool bIsAlive = true;
};
