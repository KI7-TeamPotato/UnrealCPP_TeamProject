// Fill out your copyright notice in the Description page of Project Settings.


#include "Animaiton/Notify/AnimNotifyState_AttackEnable.h"
#include "Player/TestCharacter.h"
#include "Component/WeaponComponent.h"
#include "Item/Weapon/WeaponBase.h"

void UAnimNotifyState_AttackEnable::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
    if (!Owner)
    {
        Owner = Cast<ATestCharacter>(MeshComp->GetOwner());
    }
    if (Owner)
    {
        UWeaponComponent* WeaponComponent = Owner->GetWeaponComponent();
        if (!WeaponComponent) return;
        Weapon = WeaponComponent->GetCurrentWeapon();
    }
    if (Weapon)
    {
        Weapon->BeginAttack();
    }
}

void UAnimNotifyState_AttackEnable::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    if (Weapon)
    {
        Weapon->EndAttack();
    }

    Super::NotifyEnd(MeshComp, Animation, EventReference);
}
