// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PoolingSubsystem.generated.h"

class AEnemyDamagePopupActor;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UPoolingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
    // 풀의 데미지 팝업 위젯 가져와서 해당 위치로 이동
    UFUNCTION(BlueprintCallable, Category = "PoolingSubsystem")
    void GetPooledDamagePopupActor(float DamageAmount, FVector Location);

    UFUNCTION(BlueprintCallable, Category = "PoolingSubsystem")
    void ReturnDamagePopupActorToPool(AEnemyDamagePopupActor* DamagePopupActor);
    

protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

private:
    void InitializeDamagePopupActorPool();

private:
    UPROPERTY()
    TArray<TObjectPtr<AEnemyDamagePopupActor>> AvailableDamagePopupActors;

    // 사용 가능 위젯 풀링 스택
    UPROPERTY()
    TArray<TObjectPtr<AEnemyDamagePopupActor>> UsedDamagePopupActor;

    UPROPERTY()
    int32 DamagePopupActorPoolSize = 20;
};
