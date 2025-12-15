// Fill out your copyright notice in the Description page of Project Settings.


#include "Animaiton/Animnotify_Invincible.h"
#include "Player/TestCharacter.h"

void UAnimnotify_Invincible::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (!OwnerCharacter.IsValid())
	{
		OwnerCharacter = Cast<ATestCharacter>(MeshComp->GetOwner());
	}

	if (OwnerCharacter.IsValid())
	{
		OwnerCharacter->InvincibleActivate();		//무적 활성화
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UAnimNotifyState_WeaponTrail::NotifyBegin : OwnerCharacter Unavailable"));
	}
}

void UAnimnotify_Invincible::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (OwnerCharacter.IsValid())
	{
		OwnerCharacter->InvincibleDeactivate();		//무적 비활성화
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UAnimNotifyState_WeaponTrail::NotifyBegin : OwnerCharacter가 유효하지 않습니다."));
	}
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}
