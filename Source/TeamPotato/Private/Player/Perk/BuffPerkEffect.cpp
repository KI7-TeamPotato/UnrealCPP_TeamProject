// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Perk/BuffPerkEffect.h"
#include "Player/TestCharacter.h"

void UBuffPerkEffect::OnEquip(AActor* OwnerCharacter)
{
	if (OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("BuffPerkEffect OnEquip called for %s"), *OwnerCharacter->GetName());
		
        ATestCharacter* PlayerCharacter = Cast<ATestCharacter>(OwnerCharacter);

        // 로그라이크여서 제거할 일이 없으니 모디파이어 X,
        // 만약 제거가 필요하면 Modifier 기반으로 변경
        switch (BuffType)
        {
        case EBuffType::None:
            break;
        case EBuffType::SpeedUp:
            PlayerCharacter->AddMoveSpeed(Amount);
            break;
        case EBuffType::HealthUp:
            PlayerCharacter->AddMaxHealth(Amount);
            break;
        case EBuffType::EnergyUp:
            PlayerCharacter->AddMaxEnergy(Amount);
            break;
        case EBuffType::PowerUp:
            PlayerCharacter->AddPower(Amount);
            break;
        default:
            break;
        }

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
