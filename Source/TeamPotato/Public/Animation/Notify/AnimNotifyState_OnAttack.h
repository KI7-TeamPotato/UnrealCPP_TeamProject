// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_OnAttack.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UAnimNotifyState_OnAttack : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);

private:
    TWeakObjectPtr<class ATestCharacter> OwnerCharacter = nullptr;
};
