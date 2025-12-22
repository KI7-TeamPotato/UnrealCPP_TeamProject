// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/EnumBase.h"
#include "WeaponComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMainWeaponChanged, UWeaponDataAsset*, InNewWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSubWeaponChanged, UWeaponDataAsset*, InNewWeapon);

class ATestCharacter;
class AWeaponBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMPOTATO_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void WeaponAttack();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void PickupWeapon(UWeaponDataAsset* WeaponData);

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SwapWeapon();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
	EWeaponType GetCurrentWeaponType() const;
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    inline AWeaponBase* GetCurrentWeapon() { return CurrentWeapon; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void EquipCurrentWeapon(AWeaponBase* InWeapon);

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void EquipSubWeapon(AWeaponBase* InWeapon);

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SpawnPickupWeapon(UWeaponDataAsset* WeaponData);

private:
    void BroadcastMainWeaponChanged();
    void BroadcastSubWeaponChanged();

public:
    // --- 델리게이트 ---
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnMainWeaponChanged OnMainWeaponChanged;
    
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnSubWeaponChanged OnSubWeaponChanged;

protected:
    // 플레이어
    UPROPERTY(EditAnywhere, Category = "Weapon")
    TObjectPtr<ATestCharacter> Owner = nullptr;

    // 현재 무기
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<AWeaponBase> CurrentWeapon = nullptr;

    // 보조 무기
    UPROPERTY(EditAnywhere, Category = "Weapon")
    TObjectPtr<AWeaponBase> SubWeapon = nullptr;
};
