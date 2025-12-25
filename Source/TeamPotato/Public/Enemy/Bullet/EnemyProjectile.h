// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyProjectile.generated.h"

UCLASS()
class TEAMPOTATO_API AEnemyProjectile : public AActor
{
    GENERATED_BODY()

public:
    AEnemyProjectile();

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(VisibleAnywhere, Category = "Movement")
    class UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(VisibleAnywhere, Category = "Collision")
    class USphereComponent* CollisionComp;

    UPROPERTY(VisibleAnywhere, Category = "Visual")
    class UStaticMeshComponent* MeshComp;

    UPROPERTY(EditAnywhere, Category = "Stats")
    float Damage = 10.0f;

};
