// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Object/CharacterDataObject.h"
#include "Data/CharacterDataTableRow.h"

void UCharacterDataObject::InitFromDataTableRow(const FCharacterDataTableRow& DataRow)
{
	CharacterType = DataRow.CharacterType;
	CharacterIcon = DataRow.CharacterIcon;
	CharacterIllustration = DataRow.CharacterIllustration;
}
