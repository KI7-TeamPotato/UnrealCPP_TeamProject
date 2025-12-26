// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyWeaponBase.generated.h"

UCLASS()
class TEAMPOTATO_API AEnemyWeaponBase : public AActor
{
	GENERATED_BODY()
	
    AEnemyWeaponBase();

protected:
    // 1. 스태틱 메시 컴포넌트 (무기 외형)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* WeaponMesh;

    // 2. 캡슐 콜리전 컴포넌트 (충돌 판정)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UCapsuleComponent* CollisionCapsule;

public:
    // 블루프린트의 AttackOn 이벤트 대체 함수
    // Query Only (No Physics Collision) 상태로 변경
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void EnableAttackCollision();

    // 블루프린트의 AttackOff 이벤트 대체 함수
    // No Collision 상태로 변경
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void DisableAttackCollision();
};
