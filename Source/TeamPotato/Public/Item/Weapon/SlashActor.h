// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlashActor.generated.h"

class UBoxComponent;
class UNiagaraComponent;
class UProjectileMovementComponent;

UCLASS()
class TEAMPOTATO_API ASlashActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASlashActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slash")
    TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent = nullptr;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slash")
    TObjectPtr<USceneComponent> Root = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slash")
    TObjectPtr<UBoxComponent> SlashCollision = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slash")
    TObjectPtr<UNiagaraComponent> SlashEffect = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    float SlashDamage = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
    TSubclassOf<UDamageType> DamageType = nullptr;
};
