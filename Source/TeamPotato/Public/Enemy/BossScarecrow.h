// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/BossBase.h"
#include "BossScarecrow.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScarecrowDead);

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API ABossScarecrow : public ABossBase
{
	GENERATED_BODY()
public:
    ABossScarecrow();

    UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
    FOnScarecrowDead OnScarecrowDead;
protected:
    virtual void BeginPlay() override;

    virtual void OnDie() override;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;
};
