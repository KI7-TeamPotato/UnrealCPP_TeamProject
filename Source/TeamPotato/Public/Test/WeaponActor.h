// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponActor.generated.h"

UCLASS()
class TEAMPOTATO_API AWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnWeaponBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

public:
	// 대상에게 데미지를 주는 함수
	UFUNCTION(BlueprintCallable)
	void DamageToTarget(AActor* InTarget);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> WeaponMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UCapsuleComponent> WeaponCollision = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	float Damage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TSubclassOf<UDamageType> DamageType = nullptr;
};
