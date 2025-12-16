// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ViewModel/PerkViewModel.h"

void UPerkViewModel::SetPerkDataAsset(UPerkDataAsset* NewData)
{
	//UE_LOG(LogTemp, Warning, TEXT("PerkViewModel::SetPerkDataAsset called"));
    if (!NewData) return;

    if(!CachedEquippedPerks.Contains(NewData))
    {
        CachedEquippedPerks.Add(NewData);
    }

	OnPerkEquipped.Broadcast(NewData);
}

bool UPerkViewModel::RequestEquipPerk(UPerkDataAsset* NewData, int32 SlotIndex)
{
    if (!NewData) return false;

	if (OnEquipPerkRequest.IsBound())
	{
        UE_LOG(LogTemp, Warning, TEXT("PerkViewModel::RequestEquipPerk called"));
		return OnEquipPerkRequest.Execute(NewData, SlotIndex);
	}

    return false;
}

