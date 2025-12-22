// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/AnimNotifyState_OnAttack.h"
#include "Player/TestCharacter.h"

void UAnimNotifyState_OnAttack::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (!OwnerCharacter.IsValid())
    {
        OwnerCharacter = Cast<ATestCharacter>(MeshComp->GetOwner());
    }

    if (OwnerCharacter.IsValid())
    {
        OwnerCharacter->SetOnAttacking(true);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UAnimNotifyState_OnAttack::NotifyBegin | OwnerCharacter Unavailable"));
    }
}

void UAnimNotifyState_OnAttack::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (OwnerCharacter.IsValid())
    {
        OwnerCharacter->SetOnAttacking(false);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UAnimNotifyState_OnAttack::NotifyEnd | OwnerCharacter Unavailable"));
    }

    Super::NotifyEnd(MeshComp, Animation, EventReference);
}