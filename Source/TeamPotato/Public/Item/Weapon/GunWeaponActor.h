// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/EnumBase.h"
#include "Item/Weapon/WeaponBase.h"
#include "GunWeaponActor.generated.h"

class ABulletActor;
class USkeletalMeshComponent;

UCLASS()
class TEAMPOTATO_API AGunWeaponActor : public AWeaponBase	
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGunWeaponActor();

	UFUNCTION(BlueprintCallable)
	virtual void Attack(class ATestCharacter* OwningPlayer) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	// 무기 종류
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	EWeaponType WeaponType = EWeaponType::Gun;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> GunMesh = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<ABulletActor> BulletClass;

	// 무기 공격 데미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	float AttackDamage = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float BulletSpeed = 3000.f;
};
