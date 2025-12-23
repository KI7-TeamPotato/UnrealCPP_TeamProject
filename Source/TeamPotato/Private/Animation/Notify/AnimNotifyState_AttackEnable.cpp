// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/AnimNotifyState_AttackEnable.h"
#include "Player/TestCharacter.h"
#include "Component/WeaponComponent.h"
#include "Item/Weapon/WeaponBase.h"

void UAnimNotifyState_AttackEnable::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    ATestCharacter* Owner = Cast<ATestCharacter>(MeshComp->GetOwner());
    if (!Owner) return;

    UWeaponComponent* WeaponComponent = Owner->GetWeaponComponent();
    if (!WeaponComponent) return;

    if (AWeaponBase* Weapon = WeaponComponent->GetCurrentWeapon())
    {
        Weapon->BeginAttack();
    }
}

void UAnimNotifyState_AttackEnable::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
    ATestCharacter* Owner = Cast<ATestCharacter>(MeshComp->GetOwner());
    if (!Owner) return;

    UWeaponComponent* WeaponComponent = Owner->GetWeaponComponent();
    if (!WeaponComponent) return;

    if (AWeaponBase* Weapon = WeaponComponent->GetCurrentWeapon())
    {
        Weapon->EndAttack();
    }

    Super::NotifyEnd(MeshComp, Animation, EventReference);
}
