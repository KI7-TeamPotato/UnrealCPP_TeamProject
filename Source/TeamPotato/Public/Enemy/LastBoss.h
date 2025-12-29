// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/BossBase.h"
#include "LastBoss.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameClear);

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API ALastBoss : public ABossBase
{
	GENERATED_BODY()
public:
    ALastBoss();

    UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
    FOnGameClear OnGameClear;
protected:
    virtual void BeginPlay() override;

    virtual void OnDie() override;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;
};
