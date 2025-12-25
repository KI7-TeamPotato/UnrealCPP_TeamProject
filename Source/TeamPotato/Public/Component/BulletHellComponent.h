// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BulletHellComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMPOTATO_API UBulletHellComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UBulletHellComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet|Combat")
    TSubclassOf<class AEnemyProjectile> ProjectileClass;

    UFUNCTION(BlueprintCallable, Category = "Patterns")
    void SpawnCircleSpiraPattern(int32 NumProjectiles, float Speed, float OffsetAngle = 0.0f);

    UFUNCTION(BlueprintCallable, Category = "Patterns")
    void SpawnSpiralShot(UPARAM(ref) float& CurrentAngle, float AngleStep, float Speed);

private:
    void SpawnProjectile(FVector Location, FRotator Rotation, float Speed);
};
