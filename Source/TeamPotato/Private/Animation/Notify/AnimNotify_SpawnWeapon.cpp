// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notify/AnimNotify_SpawnWeapon.h"
#include "Item/Weapon/WeaponBoxActor.h"

void UAnimNotify_SpawnWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;

    if (AWeaponBoxActor* Box =
        Cast<AWeaponBoxActor>(MeshComp->GetOwner()))
    {
        Box->OnSpawnWeaponNotify();
    }
}
