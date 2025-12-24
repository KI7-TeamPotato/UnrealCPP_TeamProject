// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/AnimNotify_OnCombo.h"
#include "Player/TestCharacter.h"

void UAnimNotify_OnCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    Super::Notify(MeshComp, Animation, EventReference);

    if (!OwnerCharacter.IsValid())
    {
        OwnerCharacter = Cast<ATestCharacter>(MeshComp->GetOwner());
    }

    if (OwnerCharacter.IsValid())
    {
        OwnerCharacter->PlaySwordAttackMontage_Combo();
    }
}
