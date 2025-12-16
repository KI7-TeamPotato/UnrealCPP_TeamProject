// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/PerkComponent.h"
#include "Player/Perk/PerkEffectBase.h"
#include "Subsystem/MVVMSubsystem.h"
#include "Kismet/GameplayStatics.h"

UPerkComponent::UPerkComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UPerkComponent::BeginPlay()
{
	Super::BeginPlay();

	EquippedPerks.Init(nullptr, MaxEquipSlots);

	if (UMVVMSubsystem* mVVMSubsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UMVVMSubsystem>())
	{
		mVVMSubsystem->RegisterPerkComp(this);
	}
}

void UPerkComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UMVVMSubsystem* mVVMSubsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UMVVMSubsystem>())
	{
		mVVMSubsystem->UnregisterPerkComp(this);
	}

	Super::EndPlay(EndPlayReason);
}

bool UPerkComponent::EquipPerk(UPerkDataAsset* PerkToEquip, int32 SlotIndex)
{
	if (!PerkToEquip) return false;

	if (IsPerkEquipped(PerkToEquip))
	{
		return false;
	}

	if (SlotIndex < 0)
	{
		for (int32 i = 0; i < EquippedPerks.Num(); i++)
		{
			if (EquippedPerks[i] == nullptr)
			{
				SlotIndex = i;
				break;
			}
		}
	}

	if (!EquippedPerks.IsValidIndex(SlotIndex) || EquippedPerks[SlotIndex] != nullptr)
	{
		return false;
	}

	EquippedPerks[SlotIndex] = PerkToEquip;

	if (PerkToEquip->PerkEffect)
	{
		UPerkEffectBase* NewEffect = DuplicateObject<UPerkEffectBase>(PerkToEquip->PerkEffect, this);

		if (NewEffect)
		{
			NewEffect->OnEquip(GetOwner());

			ActivateEffects.Add(SlotIndex, NewEffect);
		}
	}

	// VIEWMODEL에 알림 => UI 업데이트.  (만약 플레이어의 현재 상태를 따로 저장한다면 그 인스턴스에도 알림)
	UE_LOG(LogTemp, Warning, TEXT("UPerkComponent::EquipPerk - OnEquipmentUpdated.Broadcast called"));
	OnEquipmentUpdated.Broadcast(PerkToEquip);
	return true;
}

void UPerkComponent::UnequipPerk(int32 SlotIndex)
{
	if(!EquippedPerks.IsValidIndex(SlotIndex))
	{
		return;
	}

	if(EquippedPerks[SlotIndex] == nullptr)
	{
		return;
	}

	if (ActivateEffects.Contains(SlotIndex))
	{
		UPerkEffectBase* EffectToRemove = ActivateEffects[SlotIndex];
		if (EffectToRemove)
		{
			EffectToRemove->OnUnequip(GetOwner());
		}
		ActivateEffects.Remove(SlotIndex);
	}

	EquippedPerks[SlotIndex] = nullptr;
}

void UPerkComponent::ClearAllEquippedPerks()
{
	// 퍽 장착창 비우기
	for (int32 i = 0; i < EquippedPerks.Num(); i++)
	{
		UnequipPerk(i);
	}
}

bool UPerkComponent::IsPerkEquipped(UPerkDataAsset* PerkToCheck) const
{
	return EquippedPerks.Contains(PerkToCheck);
}

bool UPerkComponent::SwapEquippedPerks(int32 FromIndex, int32 ToIndex)
{
	if (!EquippedPerks.IsValidIndex(FromIndex) || !EquippedPerks.IsValidIndex(ToIndex))
	{
		return false;
	}

	EquippedPerks.Swap(FromIndex, ToIndex);

	if (ActivateEffects.Contains(FromIndex) || ActivateEffects.Contains(ToIndex))
	{
		TObjectPtr<UPerkEffectBase> TempEffect = ActivateEffects.FindRef(FromIndex);
		if (ActivateEffects.Contains(ToIndex))
		{
			ActivateEffects.Add(FromIndex, ActivateEffects[ToIndex]);
		}
		else
		{
			ActivateEffects.Remove(FromIndex);
		}
		if (TempEffect)
		{
			ActivateEffects.Add(ToIndex, TempEffect);
		}
		else
		{
			ActivateEffects.Remove(ToIndex);
		}
	}

	return true;
}

inline UPerkDataAsset* UPerkComponent::GetEquippedPerkAt(int32 SlotIndex) const
{
	if (EquippedPerks.IsValidIndex(SlotIndex))
	{
		return EquippedPerks[SlotIndex];
	}
	return nullptr;
}
