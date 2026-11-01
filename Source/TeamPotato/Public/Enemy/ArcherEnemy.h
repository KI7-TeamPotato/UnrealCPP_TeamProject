// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyCharacter.h"
#include "Enemy/BulletBase.h"
#include "ArcherEnemy.generated.h"
//이게 원거리 몬스터 베이스
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API AArcherEnemy : public AEnemyCharacter
{
	GENERATED_BODY()
public:
    AArcherEnemy();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Ranged Combat", meta = (ClampMin = "0"))
    float RetreatStartDistance = 450.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Ranged Combat", meta = (ClampMin = "0"))
    float RetreatStopDistance = 650.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Ranged Combat", meta = (ClampMin = "0"))
    float ApproachStopDistance = 900.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Ranged Combat", meta = (ClampMin = "0"))
    float MaxRangedAttackDistance = 1100.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Ranged Combat", meta = (ClampMin = "0.1"))
    float RangedAttackInterval = 3.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Ranged Combat", meta = (ClampMin = "1"))
    float RangedMovementSpeed = 500.0f;

    AActor* ResolveRangedTarget() const;
    static bool IsLivingRangedTarget(const AActor* Target);
    bool HasClearRangedShot(AActor* Target) const;
    bool HasClearRangedShotFrom(const FVector& Origin, AActor* Target) const;
    bool TryStartRangedAttack(AActor* Target);
    virtual void DefaultAttack() override;

protected:
    virtual void BeginPlay() override;

    void InitBulletPool();

public:
    // 노티파이에서 호출할 것이므로 Public + BlueprintCallable 필수
    // "이 함수를 부르면 총알이 한 발 나간다"
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void Shoot();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    TSubclassOf<ABulletBase> BulletClass;

    // 풀링된 총알들
    UPROPERTY(Transient)
    TArray<ABulletBase*> BulletPool;

    // 3~5발 정도 보유
    UPROPERTY(EditAnywhere, Category = "Combat")
    int32 PoolSize = 5;

    UPROPERTY(EditAnywhere, Category = "Combat")
    int32 BulletSpeed = 1000;

private:
    double NextRangedAttackTime = 0.0;
};
