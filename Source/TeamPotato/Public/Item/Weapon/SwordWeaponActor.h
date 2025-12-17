// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon/WeaponBase.h"
#include "Data/Enumbase.h"
#include "SwordWeaponActor.generated.h"

class UCapsuleComponent;

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API ASwordWeaponActor : public AWeaponBase
{
	GENERATED_BODY()

public:
	ASwordWeaponActor();
	
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

protected:
	// 무기 충돌 범위
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCapsuleComponent> WeaponCollision = nullptr;

	// 무기 데미지 타입
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TSubclassOf<UDamageType> DamageType = nullptr;
};
