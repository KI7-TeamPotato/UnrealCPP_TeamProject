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

    void BoxOpen();
    
protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<USkeletalMeshComponent> Mesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<USphereComponent> SphereCollision = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
    TObjectPtr<USceneComponent> SpawnLocation = nullptr;

    // 상호작용 위젯
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UWidgetComponent> InteractionWidget = nullptr;

    UPROPERTY(EditAnywhere, Category = "Weapon")
    UDataTable* WeaponDataTable;

private:
    UPROPERTY()
    bool bOpened = false;
};
