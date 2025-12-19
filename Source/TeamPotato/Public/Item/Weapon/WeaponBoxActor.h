// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IntetFace/Interactable.h"
#include "Data/WeaponPickupData.h"
#include "WeaponBoxActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBoxOpen);

class USkeletalMeshComponent;
class USphereComponent;
class UWidgetComponent;
class UWeaponDataAsset;

UCLASS()
class TEAMPOTATO_API AWeaponBoxActor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBoxActor();
    
    virtual void Interact_Implementation(AActor* InTarget) override;

    UPROPERTY(BlueprintAssignable)
    FOnBoxOpen OnBoxOpen;

    UFUNCTION()
    void SpawnRandomWeapon();

    UFUNCTION()
    void SpawnPickup(UWeaponDataAsset* InWeaponData);

    UFUNCTION()
    void OnSpawnWeaponNotify();

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

    void PlayOpenAnimation();

    void BoxOpen();
    
protected:
    // 상자 매쉬
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<USkeletalMeshComponent> Mesh = nullptr;

    // 상호작용 범위
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<USphereComponent> SphereCollision = nullptr;

    // 스폰 포인트
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<USceneComponent> SpawnLocation = nullptr;

    // 상호작용 위젯
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UWidgetComponent> InteractionWidget = nullptr;

    // 무기 데이터 테이블
    UPROPERTY(EditAnywhere, Category = "Weapon")
    TObjectPtr<UDataTable> WeaponDataTable = nullptr;

    // 상자 오픈 애니메이션
    UPROPERTY(EditAnywhere, Category = "Animation")
    TObjectPtr<UAnimMontage> OpenMontage = nullptr;

private:
    UPROPERTY()
    bool bOpened = false;
};
