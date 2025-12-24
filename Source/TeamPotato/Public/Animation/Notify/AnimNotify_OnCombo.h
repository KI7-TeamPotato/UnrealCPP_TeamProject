// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_OnCombo.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UAnimNotify_OnCombo : public UAnimNotify
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Attack")
    virtual void Notify(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference) override;

private:
    TWeakObjectPtr<class ATestCharacter> OwnerCharacter = nullptr;
};