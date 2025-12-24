// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/AnimNotifyState_CancelInput.h"
#include "Player/TestCharacter.h"

void UAnimNotifyState_CancelInput::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (!OwnerCharacter.IsValid())
    {
        OwnerCharacter = Cast<ATestCharacter>(MeshComp->GetOwner());
    }

    if (OwnerCharacter.IsValid())
    {
        OwnerCharacter->bIsComboInputAvailable = true;
    }
}

void UAnimNotifyState_CancelInput::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (OwnerCharacter.IsValid())
    {
        bool bOnCombo = OwnerCharacter->bIsOnComboInput;
        if (bOnCombo)
        {
            OwnerCharacter->bIsComboInputAvailable = false;
            OwnerCharacter->bIsOnComboInput = false;
            OwnerCharacter->PlaySwordAttackMontage_Combo();
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UAnimNotifyState_CancelInput::NotifyEnd Ownercharacter Unavailable"));
    }
}
