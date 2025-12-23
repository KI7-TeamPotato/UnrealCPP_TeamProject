// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon/SwordWeaponActor.h"
#include "RangeSwordWeaponActor.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API ARangeSwordWeaponActor : public ASwordWeaponActor
{
	GENERATED_BODY()
	
public:
    virtual void EndAttack() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slash")
    TSubclassOf<class ASlashActor> SlashActorClass;

    // 검기 충돌 방지를 위한 오너 캐릭터
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Slash")
    TObjectPtr<class ATestCharacter> OwnerCharacter = nullptr;
};
