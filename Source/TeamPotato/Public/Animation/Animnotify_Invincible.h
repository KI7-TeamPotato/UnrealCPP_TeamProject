// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Animnotify_Invincible.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UAnimnotify_Invincible : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
    virtual void NotifyBegin(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        float TotalDuration,
        const FAnimNotifyEventReference& EventReference
    ) override;

    virtual void NotifyEnd(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference
    ) override;

private:
    TWeakObjectPtr<class ATestCharacter> OwnerCharacter = nullptr;
};
