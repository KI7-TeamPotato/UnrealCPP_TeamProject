// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ViewModel/WeaponViewModel.h"

void UWeaponViewModel::SetResource(float CurrentResource, float MaxResource)
{
    ResourcePercent = FMath::Clamp(CurrentResource / MaxResource, 0.0f, 1.0f);
    OnPlayerResourceChanged.Broadcast(ResourcePercent);
}

void UWeaponViewModel::SetMainWeapon(FText InWeaponName, UTexture2D* InWeaponIcon)
{
    MainWeapon.WeaponName = InWeaponName;
    MainWeapon.WeaponIcon = InWeaponIcon;
    OnMainWeaponChanged.Broadcast(MainWeapon.WeaponName, MainWeapon.WeaponIcon);
}

void UWeaponViewModel::SetSubWeapon(FText InWeaponName, UTexture2D* InWeaponIcon)
{
    MainWeapon.WeaponName = InWeaponName;
    MainWeapon.WeaponIcon = InWeaponIcon;
    OnSubWeaponChanged.Broadcast(MainWeapon.WeaponName, MainWeapon.WeaponIcon);
}

void UWeaponViewModel::SwapMainAndSubWeapon()
{

}
