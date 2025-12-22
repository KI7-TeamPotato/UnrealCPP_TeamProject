// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ViewModel/WeaponViewModel.h"
#include "Data/WeaponDataAsset.h"

void UWeaponViewModel::SetResource(float CurrentResource, float MaxResource)
{
    if (OnPlayerResourceUpdate.IsBound())
    {
        ResourcePercent = FMath::Clamp(CurrentResource / MaxResource, 0.0f, 1.0f);
        OnPlayerResourceUpdate.Broadcast(ResourcePercent);
    }
}

void UWeaponViewModel::SetMainWeapon(UWeaponDataAsset* InWeaponData)
{
    if (!InWeaponData) return;

    if (OnMainWeaponUpdate.IsBound())
    {
        OnMainWeaponUpdate.Broadcast(InWeaponData->WeaponName, InWeaponData->WeaponIcon);
    
    }
}

void UWeaponViewModel::SetSubWeapon(UWeaponDataAsset* InWeaponData)
{
    if (!InWeaponData) return;

    if (OnSubWeaponUpdate.IsBound())
    {
        OnSubWeaponUpdate.Broadcast(InWeaponData->WeaponIcon);
    }
}

void UWeaponViewModel::SwapMainAndSubWeapon()
{

}
