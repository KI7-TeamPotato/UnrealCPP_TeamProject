// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Object/PerkDataObject.h"

void UPerkDataObject::InitFromDataTableAsset(UPerkDataAsset* PerKDataAsset)
{
	Name = PerKDataAsset->PerkName;
	PerkIcon = PerKDataAsset->Icon;
	PerkDescription = PerKDataAsset->Description;
}
