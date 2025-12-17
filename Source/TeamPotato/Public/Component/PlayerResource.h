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


private:
	inline bool IsStaminaRemain(float InUseStaminaAmount) { return (Stamina > InUseStaminaAmount); }

private:
	//체력
	float Health = 100.0f;
	//최대 체력
	const float MaxHealth = 100.0f;
    //사망 확인 기준점. 체력이 이보다 작을 시 사망처리함
    const float MinHealth = 0.0001;

	//스태미나
	float Stamina = 100.0f;
	//최대 스태미나
	const float MaxStamina = 100.0f;

	//공격력
	float AttackPower = 10.0f;
	//최소 공격력(이 이하로 내려가지 않음)
	float MinAttackPower = 0.1f;

	//살아있는지 죽었는지
	bool bIsAlive = true;
};
