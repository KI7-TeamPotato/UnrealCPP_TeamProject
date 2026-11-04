// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/AnimNotifyState_OnAttack.h"
#include "Player/TestCharacter.h"

void UAnimNotifyState_OnAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (auto* Owner = MeshComp ? Cast<ATestCharacter>(MeshComp->GetOwner()) : nullptr)
    {
        Owner->SetOnAttacking(true);
    }
}

void UAnimNotifyState_OnAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (auto* Owner = MeshComp ? Cast<ATestCharacter>(MeshComp->GetOwner()) : nullptr)
    {
        Owner->SetOnAttacking(false);
    }

    Super::NotifyEnd(MeshComp, Animation, EventReference);
}
