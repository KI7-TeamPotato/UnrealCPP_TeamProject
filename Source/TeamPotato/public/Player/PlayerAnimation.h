// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerAnimation.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMPOTATO_API UPlayerAnimation : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerAnimation();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

protected:
	//Anim Montage
	//구르기 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|Montage")
	TObjectPtr<class UAnimMontage> RollMontage = nullptr;
	//검 공격 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animaiton|Montage")
	TObjectPtr<class UAnimMontage> SwordAttackMontage = nullptr;

};
