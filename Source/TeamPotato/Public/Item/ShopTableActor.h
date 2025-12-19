// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IntetFace/Interactable.h"
#include "ShopTableActor.generated.h"

class APickupActor;
class UWidgetComponent;
class UShopInteractWidget;
class USphereComponent;
class UShopInteractWidget;

UCLASS()
class TEAMPOTATO_API AShopTableActor : public AActor, public IInteractable
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AShopTableActor();

    virtual void Interact_Implementation(AActor* InTarget) override;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    void BuyItem(AActor* InPlayer);

protected:
    // 테이블 매쉬
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
    TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

    // 아이템 스폰 포인트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
    TObjectPtr<USceneComponent> SpawnPoint = nullptr;

    // 상호작용 범위
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<USphereComponent> SphereCollision = nullptr;

    // 상호작용 위젯 컴포넌트
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UWidgetComponent> InteractionWidget = nullptr;

    // 위젯 컴포넌트 클래스
    UPROPERTY(EditAnywhere, Category = "Shop")
    TSubclassOf<UShopInteractWidget> InteractionWidgetClass;

    // 스폰 아이템
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
    TSubclassOf<APickupActor> SellItem;

    // 블루프린트에서 쓸 아이템 가격
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shop")
    int32 Price = 0;

    // 무기 스폰 데이터 테이블
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
    TObjectPtr<UDataTable> WeaponDataTable = nullptr;

private:
    UPROPERTY()
    TObjectPtr<APickupActor> SpawnItem = nullptr;
};
