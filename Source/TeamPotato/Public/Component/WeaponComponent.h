// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/EnumBase.h"
#include "WeaponComponent.generated.h"

class ATestCharacter;
class AWeaponBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMPOTATO_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void WeaponAttack();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(UWeaponDataAsset* WeaponData);

	EWeaponType GetCurrentWeaponType() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:
    // 플레이어
    UPROPERTY(EditAnywhere, Category = "Weapon")
    TObjectPtr<ATestCharacter> Owner = nullptr;

    // 현재 무기
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<AWeaponBase> CurrentWeapon = nullptr;
};
