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
	void EquipWeapon(TSubclassOf<AWeaponBase> InWeapon);

	EWeaponType GetCurrentWeaponType() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> OwnerMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AWeaponBase> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<AWeaponBase> CurrentWeapon = nullptr;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<ATestCharacter> Owner = nullptr;
};
