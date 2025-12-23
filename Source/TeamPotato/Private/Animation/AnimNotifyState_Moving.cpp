// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_Moving.h"
#include "Player/TestCharacter.h"

void UAnimNotifyState_Moving::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (!OwnerCharacter.IsValid())
    {
        OwnerCharacter = Cast<ATestCharacter>(MeshComp->GetOwner());
    }

    if(OwnerCharacter.IsValid())
    {
        OwnerCharacter->SetAnimRootMotionIgnore();
        UE_LOG(LogTemp, Log, TEXT("RootMotion Ignored"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UAnimNotifyState_Moving::NotifyBegin | OwnerCharacter Unavailable"));
    }
}

void UAnimNotifyState_Moving::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (OwnerCharacter.IsValid())
    {
        OwnerCharacter->SetAnimRootMotionFromMontage();
        UE_LOG(LogTemp, Log, TEXT("RootMotion Unignored"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UAnimNotifyState_Moving::NotifyEnd | OwnerCharacter Unavailable"));
    }

    Super::NotifyEnd(MeshComp, Animation, EventReference);
}
