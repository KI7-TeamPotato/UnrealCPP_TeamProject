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
	inline float GetHealthAmount() { return Health; }
	inline float GetStaminaAmount() { return Stamina; }

	void TakeDamage(float InDamage);
	void Heal(float InHeal);
	void UseStamina(float InUseStaminaAmount);

private:
	//체력
	float Health = 100.0f;
	//최대 체력
	const float MaxHealth = 100.0f;

	//스태미나
	float Stamina = 100.0f;
	//최대 스태미나
	const float MaxStamina = 100.0f;

	//공격력
	float AttackPower = 10.0f;
	//최소 공격력(이 이하로 내려가지 않음)
	float MinAttackPower = 0.1f;

	//살아있는지
	bool bIsAlive = true;
};
