// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/MVVMSubsystem.h"
#include "Component/PerkComponent.h"
#include "Subsystem/ViewModel/PlayerStatusViewModel.h"
#include "Subsystem/ViewModel/PerkViewModel.h"

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


// ==============================================================================
// 컴포넌트 등록 및 해제 함수들
// ==============================================================================

/*
void UMVVMSubsystem::RegisterPlayerResourceComp(UPlayerResource* NewComp)
{
	if (!NewComp) return;

	// 기존에 등록된 컴포넌트가 있다면 해제(안정장치)
	UnregisterPlayerResourceComp(NewComp);

	// 뷰모델 가져오기
	UPlayerStatusViewModel* VM = GetPlayerStatusViewModel();

	// 델리게이트 바인딩(컴포넌트의 체력이 바뀌면 -> 뷰모델의 SetHealth도 실행 등등)
	NewComp->OnHealthChanged.AddDynamic(PlayerStatusViewModel, &UPlayerStatusViewModel::SetHealth);
	NewComp->OnResourceChanged.AddDynamic(PlayerStatusViewModel, &UPlayerStatusViewModel::SetResource);
}

void UMVVMSubsystem::UnregisterPlayerResourceComp(UPlayerResource* ExitingComp)
{
	//if (ExitingComp && PlayerStatusViewModel)
	//{
	//	// 델리게이트 언바인딩
	//	ExitingComp->OnHealthChanged.RemoveDynamic(PlayerStatusViewModel, &UPlayerStatusViewModel::SetHealth);
	//	ExitingComp->OnResourceChanged.RemoveDynamic(PlayerStatusViewModel, &UPlayerStatusViewModel::SetResource);
	//}
}
*/

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
