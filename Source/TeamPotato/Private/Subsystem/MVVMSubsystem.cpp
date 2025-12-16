// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/MVVMSubsystem.h"
#include "Component/PerkComponent.h"
#include "Subsystem/ViewModel/PlayerStatusViewModel.h"
#include "Subsystem/ViewModel/SkillViewModel.h"
#include "UI/Player/PlayerStatWidget.h"
#include "UI/Perk/PerkSelectionWidget.h"

UPlayerStatusViewModel* UMVVMSubsystem::GetPlayerStatusViewModel()
{
	if (!PlayerStatusViewModel)
	{
		PlayerStatusViewModel = NewObject<UPlayerStatusViewModel>(this);
	}	
	return PlayerStatusViewModel;
}

USkillViewModel* UMVVMSubsystem::GetSkillViewModel()
{
	if (!SkillViewModel)
	{
		SkillViewModel = NewObject<USkillViewModel>(this);
	}
	return SkillViewModel;
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
	GetPlayerStatusViewModel();

	// 델리게이트 바인딩(컴포넌트의 체력이 바뀌면 -> 뷰모델의 SetHealth도 실행 등등)
	NewComp->OnHealthChanged.AddDynamic(PlayerStatusViewModel, &UPlayerStatusViewModel::SetHealth);
	NewComp->OnResourceChanged.AddDynamic(PlayerStatusViewModel, &UPlayerStatusViewModel::SetResource);
	
	// 초기 값 설정
	PlayerStatusViewModel->SetHealth(NewComp->GetCurrentHealth(), NewComp->GetMaxHealth());
	PlayerStatusViewModel->SetResource(NewComp->GetCurrentResource(), NewComp->GetMaxResource());
	PlayerStatusViewModel->SetPlayerIcon(NewComp->GetPlayerIcon()); // 아이콘 설정은 플레이어 셀렉션 위젯에서 처리
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
	GetSkillViewModel();

	// 델리게이트 바인딩
	NewComp->OnEquipmentUpdated.AddDynamic(SkillViewModel, &USkillViewModel::SetPerkDataAsset);
	SkillViewModel->OnEquipPerkRequest.BindDynamic(NewComp, &UPerkComponent::EquipPerk);
}

void UMVVMSubsystem::UnregisterPerkComp(UPerkComponent* ExitingComp)
{
	if (ExitingComp && SkillViewModel)
	{
		// 델리게이트 언바인딩
		ExitingComp->OnEquipmentUpdated.RemoveDynamic(SkillViewModel, &USkillViewModel::SetPerkDataAsset);
		SkillViewModel->OnEquipPerkRequest.BindDynamic(ExitingComp, &UPerkComponent::EquipPerk);
	}
}


// ==============================================================================
// 위젯 등록 및 해제 함수들
// ==============================================================================

void UMVVMSubsystem::ResgisterPlayerStatWidget(UPlayerStatWidget* NewWidget)
{
}

void UMVVMSubsystem::UnregisterPlayerStatWidget(UPlayerStatWidget* ExitingWidget)
{
}

void UMVVMSubsystem::ResgisterPerkSelectionWidget(UPerkSelectionWidget* NewWidget)
{
	if (!NewWidget) return;

	UnregisterPerkSelectionWidget(NewWidget);

	// 뷰모델 가져오기
	GetSkillViewModel();

	// 델리게이트 바인딩
	NewWidget->OnTryEquippedPerk.AddDynamic(SkillViewModel, &USkillViewModel::EquippedPerkInPerkComp);
}

void UMVVMSubsystem::UnregisterPerkSelectionWidget(UPerkSelectionWidget* ExitingWidget)
{
	if (ExitingWidget && SkillViewModel)
	{
		// 델리게이트 언바인딩
		ExitingWidget->OnTryEquippedPerk.RemoveDynamic(SkillViewModel, &USkillViewModel::EquippedPerkInPerkComp);
	}
}

