// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyDamagePopupActor.generated.h"

class UWidgetComponent;

UCLASS()
class TEAMPOTATO_API AEnemyDamagePopupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemyDamagePopupActor();

protected:
	virtual void BeginPlay() override;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UWidgetComponent> DamagePopupWidgetComponent;
};
