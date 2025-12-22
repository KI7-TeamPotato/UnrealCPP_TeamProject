// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "EnemyAxe.generated.h"

// 무기 상태 정의
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
    Idle,       // 손에 있음
    Thrown,     // 날아가는 중
    Returning   // 돌아오는 중 (플레이어를 끌고 올 수도 있음)
};

UCLASS()
class TEAMPOTATO_API AEnemyAxe : public AActor
{
    GENERATED_BODY()

public:
    AEnemyAxe();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override; // 이동 로직은 여기서 처리

public:
    // 무기 던지기 함수 (목표 위치)
    UFUNCTION(BlueprintCallable)
    void ThrowAt(FVector TargetLocation);

    // 충돌 처리
    UFUNCTION()
    void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    UBoxComponent* CollisionBox;

protected:
    // 현재 상태
    EWeaponState CurrentState = EWeaponState::Idle;

    // 던질 목표 위치
    FVector ThrowTargetLocation;

    // 원래 주인 (적)
    AActor* WeaponOwner;

    // 끌려오는 플레이어 (없으면 nullptr)
    ACharacter* CaughtPlayer;

    // 무기 비행 속도
    UPROPERTY(EditAnywhere, Category = "Combat")
    float ThrowSpeed = 1500.0f;

    // 돌아오는 속도 (끌고 올 때)
    UPROPERTY(EditAnywhere, Category = "Combat")
    float ReturnSpeed = 2000.0f;
};