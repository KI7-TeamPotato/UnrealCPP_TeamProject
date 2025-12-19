// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Intetface/Pickupable.h"
#include "PickupActor.generated.h"

UCLASS()
class TEAMPOTATO_API APickupActor : public AActor, public IPickupable
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    APickupActor();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
    // 아이템 획득 함수
    virtual void OnPickup(AActor* InPlayer) override;

    // 아이템 적용 함수
    virtual void UseItem(AActor* InPlayer) {}

    // 아이템 판매 상태 변화 함수
    void SetSellItem(bool IsSell);

    virtual int32 GetPrice() const;

private:
    UFUNCTION()
    void OnTimeLineUpdate(float Value);

    UFUNCTION()
    void OnTimelineFinished();

public:
    UPROPERTY()
    bool bPickedUp = false;

    UPROPERTY()
    bool bIsSell = false;

protected:
	// 아이템 획득 범위
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<class USphereComponent> SphereCollision;

	// 아이템 매쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<class UStaticMeshComponent> Mesh;

    // 아이템 회전 속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    float RotateSpeed = 180.0f;

    // 획득 스케일 타임라인 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<class UTimelineComponent> PickupTimeline = nullptr;

    // 스폰 후에 먹을 수 있기 까지의 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    float PickupableTime = 2.0f;

    // 픽업 획득 효과용 스케일 커브
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
    TObjectPtr<UCurveFloat> ScaleCurve = nullptr;

    // 픽업 획득 효과용 거리 보간 커브
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
    TObjectPtr<UCurveFloat> DistanceCurve = nullptr;

    // 픽업 획득 효과용 높이 커브
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
    TObjectPtr<UCurveFloat> HeightCurve = nullptr;

    // 아이템 획득에 걸리는 시간(초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    float Duration = 0.5f;

    // 아이템 획득 시 튀어오르는 높이
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
    float PickupHeight = 50.0f;

    // 아이템 가격
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    int32 ItemPrice = 0;
    
private:
    // 이 픽업 아이템을 먹은 액터
    UPROPERTY()
    TObjectPtr<AActor> PickupOwner = nullptr;

    // 획득했을 때 메시 월드 위치
    FVector PickupStartLocation;

    // 스폰 직후 먹지 못하는 시간동안 기다리는 것을 처리하는 타이머 핸들
    FTimerHandle PickupableTimer;
};
