// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/EnumBase.h"
#include "Components/ActorComponent.h"
#include "PlayerAnimation.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMPOTATO_API UPlayerAnimation : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerAnimation();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
	void PlayRollMontage();

	UFUNCTION()
	void PlayAttackAnimation();

private:
	
	EWeaponType PlayerWeapon = EWeaponType::None;

	//메인 플레이어 포인터
	UPROPERTY()
	TWeakObjectPtr<class ATestCharacter> MainPlayer = nullptr;
};
