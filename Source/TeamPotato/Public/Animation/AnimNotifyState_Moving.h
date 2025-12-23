// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_Moving.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UAnimNotifyState_Moving : public UAnimNotifyState
{
	GENERATED_BODY()
	
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
    //플레이어
    TWeakObjectPtr<class ATestCharacter> OwnerCharacter = nullptr;
};
