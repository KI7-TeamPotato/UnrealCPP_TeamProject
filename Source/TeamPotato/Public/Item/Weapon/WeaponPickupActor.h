// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IntetFace/Interactable.h"
#include "Item/PickupActor.h"
#include "WeaponPickupActor.generated.h"

class AWeaponBase;
class UWidgetComponent;
class AWeaponBoxActor;
class UWeaponDataAsset;

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API AWeaponPickupActor : public APickupActor, public IInteractable
{
	GENERATED_BODY()
	
public:
	AWeaponPickupActor();

    // 무기 데이터 설정
    void SetWeaponData(UWeaponDataAsset* InData);

    // 상호작용 인터페이스
    virtual void Interact_Implementation(AActor* InTarget) override;

	// 아이템 획득 함수
	virtual void OnPickup(AActor* InPlayer) override;

    // 가격 조회 함수
    virtual int32 GetPrice() const override;

    // 무기 획득 후 파괴를 위한 스폰 상자를 설정
    void SetSourceBox(AWeaponBoxActor* InBox)
    {
        SourceBox = InBox;
    }

protected:
	virtual void BeginPlay() override;

	// 무기 아이템인지 확인하여 true 리턴
	virtual bool ItemInteraction() const override { return true; }

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
    // 무기 데이터
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<UWeaponDataAsset> WeaponData;

	// 상호작용 위젯
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* InteractionWidget;

    // 이 무기를 스폰한 상자
    UPROPERTY()
    TObjectPtr<AWeaponBoxActor> SourceBox;
};
