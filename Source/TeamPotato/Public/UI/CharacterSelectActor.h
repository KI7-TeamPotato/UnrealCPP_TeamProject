// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Intetface/Interactable.h"
#include "CharacterSelectActor.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UWidgetComponent;
class UShopInteractWidget;
class UCharacterSelectMenu;


UCLASS()
class TEAMPOTATO_API ACharacterSelectActor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACharacterSelectActor();

    virtual void Interact_Implementation(AActor* InTarget) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    void SelectCharacter(AActor* InPlayer);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
    // 매쉬
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UStaticMeshComponent> Mesh = nullptr;

    // 상호작용 범위
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<USphereComponent> SphereCollision = nullptr;

    // 상호작용 위젯 컴포넌트
    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UWidgetComponent> InteractionWidget = nullptr;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UCharacterSelectMenu> CharacterSelectMenuClass;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UCharacterSelectMenu> CharacterSelectMenuInstance = nullptr;
};
