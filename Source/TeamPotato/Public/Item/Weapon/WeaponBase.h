// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/EnumBase.h"
#include "WeaponBase.generated.h"

class UWeaponComponent;
class UPlayerResource;

UCLASS()
class TEAMPOTATO_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// 공격 함수
    virtual void Attack(class ATestCharacter* OwningPlayer);

	inline void SetOwnerComponent(UWeaponComponent* InWeaponComponent) { OwnerWeaponComponent = InWeaponComponent; }

	EWeaponType GetWeaponType() const { return WeaponType; }

protected:
	// 무기 타입
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	EWeaponType WeaponType = EWeaponType::None;

	// 무기 매쉬
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh = nullptr;

    // 무기 공격 데미지
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    float AttackDamage = 10.0f;

    // 무기 공격 소모 
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    float AttackCost = 1.0f;

	// 웨폰 컴포넌트
	UPROPERTY()
	TObjectPtr<UWeaponComponent> OwnerWeaponComponent = nullptr;

public:
	bool bIsActivated = false;
};
