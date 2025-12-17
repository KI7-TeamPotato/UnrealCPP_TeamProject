// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IntetFace/Interactable.h"
#include "Item/PickupActor.h"
#include "WeaponPickupActor.generated.h"

class AWeaponBase;
class UWidgetComponent;

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API AWeaponPickupActor : public APickupActor, public IInteractable
{
	GENERATED_BODY()
	
public:
	AWeaponPickupActor();

    // 상호작용 인터페이스
    virtual void Interact_Implementation(AActor* InTarget) override;

	// 아이템 획득 함수
	virtual void OnPickup(AActor* InPlayer) override;

    inline void SetWeaponClass(TSubclassOf<AWeaponBase> InWeaponClass)
    {
        WeaponClass = InWeaponClass;
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
	// 무기 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AWeaponBase> WeaponClass;

	// 상호작용 위젯
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent* InteractionWidget;
};
