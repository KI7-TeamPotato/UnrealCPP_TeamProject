// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ViewModel/SkillViewModel.h"

void USkillViewModel::SetPerkDataAsset(UPerkDataAsset* NewData)
{
	//UE_LOG(LogTemp, Warning, TEXT("SkillViewModel::SetPerkDataAsset called"));
	OnPerkDataAssetChanged.Broadcast(NewData);
}

void USkillViewModel::EquippedPerkInPerkComp(UPerkDataAsset* NewData)
{
	UE_LOG(LogTemp, Warning, TEXT("SkillViewModel::EquippedPerkInPerkComp called"));
	if (OnEquipPerkRequest.IsBound())
	{
		bool bSuccess = OnEquipPerkRequest.Execute(NewData, -1);
	}
}

