// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Perk/BuffPerkEffect.h"
#include "Player/TestCharacter.h"

void UBuffPerkEffect::OnEquip(AActor* OwnerCharacter)
{
	if (OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("BuffPerkEffect OnEquip called for %s"), *OwnerCharacter->GetName());
		
		// 플레이어와 연결할때 풀기. Modifier에 더하는 기능
		/*
		ATeamproject_UICharacter* PlayerCharacter = Cast<ATeamproject_UICharacter>(OwnerCharacter);
		PlayerCharacter->AddFlatModifier(BuffAmount);
		*/
	}
}

void UBuffPerkEffect::OnUnequip(AActor* OwnerCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("BuffPerkEffect OnUnequip called for %s"), *OwnerCharacter->GetName());
}
