// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/EnumBase.h"
#include "WeaponComponent.generated.h"

class AGunWeaponActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMPOTATO_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void FireWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(TSubclassOf<AGunWeaponActor> InWeapon);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponType WeaponData = EWeaponType::Gun;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> OwnerMesh = nullptr;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AGunWeaponActor> GunClass;

	UPROPERTY()
	TObjectPtr<AGunWeaponActor> CurrentGun = nullptr;
};
