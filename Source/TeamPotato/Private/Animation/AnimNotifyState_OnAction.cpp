// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifyState_OnAction.h"
#include "Player/TestCharacter.h"

void UAnimNotifyState_OnAction::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!OwnerCharacter.IsValid())
	{
		OwnerCharacter = Cast<ATestCharacter>(MeshComp->GetOwner());
	}

	if (OwnerCharacter.IsValid())
	{
		OwnerCharacter->SetOnActing(true);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UAnimNotifyState_OnAction::NotifyBegin OwnerCharacter Unavailable"));
	}
}

void UAnimNotifyState_OnAction::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (OwnerCharacter.IsValid())
	{
		OwnerCharacter->SetOnActing(false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UAnimNotifyState_OnAction::NotifyBegin OwnerCharacter Unavailable"));
	}
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
