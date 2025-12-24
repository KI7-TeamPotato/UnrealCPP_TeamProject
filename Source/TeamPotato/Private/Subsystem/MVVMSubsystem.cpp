// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/MVVMSubsystem.h"
#include "Component/PerkComponent.h"
#include "Component/PlayerResource.h"
#include "Component/WeaponComponent.h"
#include "Subsystem/ViewModel/PlayerStatusViewModel.h"
#include "Subsystem/ViewModel/PerkViewModel.h"
#include "Subsystem/ViewModel/WeaponViewModel.h"
#include "Subsystem/ViewModel/ItemViewModel.h"
#include "Subsystem/ViewModel/MinimapViewModel.h"
#include "Data/WeaponDataAsset.h"
#include "TeamPotato/Logic/DungeonGanarator.h"

UPlayerStatusViewModel* UMVVMSubsystem::GetPlayerStatusViewModel()
{
	if (!PlayerStatusViewModel)
	{
		PlayerStatusViewModel = NewObject<UPlayerStatusViewModel>(this);
	}	
	return PlayerStatusViewModel;
}

UPerkViewModel* UMVVMSubsystem::GetPerkViewModel()
{
	if (!PerkViewModel)
	{
        PerkViewModel = NewObject<UPerkViewModel>(this);
	}
	return PerkViewModel;
}

UWeaponViewModel* UMVVMSubsystem::GetWeaponViewModel()
{
    if (!WeaponViewModel)
    {
        WeaponViewModel = NewObject<UWeaponViewModel>(this);
    }
    return WeaponViewModel;
}

UItemViewModel* UMVVMSubsystem::GetItemViewModel()
{
    if (!ItemViewModel)
    {
        ItemViewModel = NewObject<UItemViewModel>(this);
    }
    return ItemViewModel;

UMinimapViewModel* UMVVMSubsystem::GetMinimapViewModel()
{
    if (!MinimapViewModel)
    {
        MinimapViewModel = NewObject<UMinimapViewModel>(this);
    }
    return MinimapViewModel;
}


// ==============================================================================
// 컴포넌트 등록 및 해제 함수들
// ==============================================================================

void UMVVMSubsystem::RegisterPlayerResourceComp(UPlayerResource* NewComp)
{
	if (!NewComp) return;

	// 기존에 등록된 컴포넌트가 있다면 해제(안정장치)
	UnregisterPlayerResourceComp(NewComp);

	// 뷰모델 가져오기
	UPlayerStatusViewModel* VM = GetPlayerStatusViewModel();

	// 델리게이트 바인딩(컴포넌트의 체력이 바뀌면 -> 뷰모델의 SetHealth도 실행 등등)
	NewComp->OnHealthChanged.AddDynamic(PlayerStatusViewModel, &UPlayerStatusViewModel::SetHealth);
    NewComp->OnEnergyChanged.AddDynamic(WeaponViewModel, &UWeaponViewModel::SetResource);
    NewComp->OnGoldChanged.AddDynamic(ItemViewModel, &UItemViewModel::SetCurrentGold);
}

void UMVVMSubsystem::UnregisterPlayerResourceComp(UPlayerResource* ExitingComp)
{
	if (ExitingComp && PlayerStatusViewModel)
	{
		// 델리게이트 언바인딩
		ExitingComp->OnHealthChanged.RemoveDynamic(PlayerStatusViewModel, &UPlayerStatusViewModel::SetHealth);
        ExitingComp->OnEnergyChanged.RemoveDynamic(WeaponViewModel, &UWeaponViewModel::SetResource);	
	}
}


void UMVVMSubsystem::RegisterPerkComp(UPerkComponent* NewComp)
{
	if (!NewComp) return;

	UnregisterPerkComp(NewComp);

	// 뷰모델 가져오기
    UPerkViewModel* VM = GetPerkViewModel();

	// Model->ViewModel (장착된 퍽이 바뀌면 뷰모델에 반영)
	NewComp->OnEquipmentUpdated.AddDynamic(VM, &UPerkViewModel::SetPerkDataAsset);

    // ViewModel->Model (장착 요청시에 컴포넌트에서 장착)
    VM->OnEquipPerkRequest.BindDynamic(NewComp, &UPerkComponent::EquipPerk);
}

void UMVVMSubsystem::UnregisterPerkComp(UPerkComponent* ExitingComp)
{
	if (ExitingComp && PerkViewModel)
	{
		// 델리게이트 언바인딩
		ExitingComp->OnEquipmentUpdated.RemoveDynamic(PerkViewModel, &UPerkViewModel::SetPerkDataAsset);
        PerkViewModel->OnEquipPerkRequest.Unbind();
	}
}

void UMVVMSubsystem::RegisterWeaponComp(UWeaponComponent* NewComp)
{
    if (!NewComp) return;

    UnregisterWeaponComp(NewComp);

    // 뷰모델 가져오기
    UWeaponViewModel* VM = GetWeaponViewModel();


    // Model->ViewModel (무기가 바뀌면 뷰모델에 반영)
    // 1. 메인 무기 변경
    // 2. 서브 무기 변경
    
    // Model->ViewModel (장착된 무기가 바뀌면 뷰모델에 반영)
    NewComp->OnMainWeaponChanged.AddDynamic(VM, &UWeaponViewModel::SetMainWeapon);
    NewComp->OnSubWeaponChanged.AddDynamic(VM, &UWeaponViewModel::SetSubWeapon);
}

void UMVVMSubsystem::UnregisterWeaponComp(UWeaponComponent* ExitingComp)
{
    if (ExitingComp && WeaponViewModel)
    {
        // 델리게이트 언바인딩
        ExitingComp->OnMainWeaponChanged.RemoveDynamic(WeaponViewModel, &UWeaponViewModel::SetMainWeapon);
        ExitingComp->OnSubWeaponChanged.RemoveDynamic(WeaponViewModel, &UWeaponViewModel::SetSubWeapon);
    }
}

void UMVVMSubsystem::RegisterDungeonGeneratorActor(ADungeonGanarator* NewActor)
{
    if (!NewActor) return;

    UnregisterDungeonGeneratorActor(NewActor);

    // 뷰모델 가져오기
    UMinimapViewModel* VM = GetMinimapViewModel();

    // DungeonGenerator -> ViewModel -> 던전 생성 완료 알림 뿌림
    NewActor->OnDungeonGenerationCompleted.AddDynamic(VM, &UMinimapViewModel::RequestMinimapCapture);
}

void UMVVMSubsystem::UnregisterDungeonGeneratorActor(ADungeonGanarator* ExitingActor)
{
    if (ExitingActor && MinimapViewModel)
    {
        // 델리게이트 언바인딩
        ExitingActor->OnDungeonGenerationCompleted.RemoveDynamic(MinimapViewModel, &UMinimapViewModel::RequestMinimapCapture);
    }
}

