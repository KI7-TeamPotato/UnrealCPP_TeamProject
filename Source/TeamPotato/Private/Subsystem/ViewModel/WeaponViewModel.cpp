// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ViewModel/WeaponViewModel.h"
#include "Data/WeaponDataAsset.h"

void UWeaponViewModel::SetResource(float CurrentResource, float MaxResource)
{
    if (OnPlayerResourceUpdate.IsBound())
    {
        OnPlayerResourceUpdate.Broadcast(CurrentResource, MaxResource);
    }
}

void UWeaponViewModel::SetMainWeapon(UWeaponDataAsset* InWeaponData)
{
    if (!InWeaponData) return;

    if (OnMainWeaponUpdate.IsBound())
    {
        OnMainWeaponUpdate.Broadcast(InWeaponData);
    
    }
}

void UWeaponViewModel::SetSubWeapon(UWeaponDataAsset* InWeaponData)
{
    if (!InWeaponData) return;

    if (OnSubWeaponUpdate.IsBound())
    {
        OnSubWeaponUpdate.Broadcast(InWeaponData);
    }
}

void UWeaponViewModel::SwapMainAndSubWeapon()
{

}
