// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/AnimNotifyState_CancelInput.h"
#include "Animation/AnimNotifyLibrary.h"
#include "Player/TestCharacter.h"

void UAnimNotifyState_CancelInput::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    auto* Owner = MeshComp ? Cast<ATestCharacter>(MeshComp->GetOwner()) : nullptr;
    if (Owner && Owner->IsCurrentSwordAttack(Animation))
    {
        Owner->bIsComboInputAvailable = true;
    }
}

void UAnimNotifyState_CancelInput::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    auto* Owner = MeshComp ? Cast<ATestCharacter>(MeshComp->GetOwner()) : nullptr;
    if (Owner && Owner->IsCurrentSwordAttack(Animation))
    {
        Owner->bIsComboInputAvailable = false;
        if (UAnimNotifyLibrary::NotifyStateReachedEnd(EventReference))
        {
            Owner->PlaySwordAttackMontage_Combo();
        }
        else
        {
            Owner->bIsOnComboInput = false;
        }
    }
    Super::NotifyEnd(MeshComp, Animation, EventReference);
}
