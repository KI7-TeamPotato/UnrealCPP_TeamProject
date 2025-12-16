// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/EnumBase.h"
#include "Item/Weapon/WeaponBase.h"
#include "TestWeapon.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API ATestWeapon : public AWeaponBase
{
	GENERATED_BODY()
	
public:
	ATestWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 오버랩 시작시 실행되는 함수
	UFUNCTION()
	void OnWeaponBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

public:
	// 대상에게 데미지를 주는 함수
	UFUNCTION(BlueprintCallable)
	void DamageToTarget(AActor* InTarget);

public:

protected:
	// 무기 종류
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	EWeaponType WeaponType = EWeaponType::Sword;

	// 무기 매쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> WeaponMesh = nullptr;

	// 무기 충돌 범위
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UCapsuleComponent> WeaponCollision = nullptr;

	// 무기 공격 데미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	float AttackDamage = 10.0f;

	// 무기 데미지 타입
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TSubclassOf<UDamageType> DamageType = nullptr;

private:
	TObjectPtr<class APlayerAttack> ExecutePlayerAttack = nullptr;
};
