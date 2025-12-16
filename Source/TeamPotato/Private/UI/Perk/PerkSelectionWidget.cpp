// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Perk/PerkSelectionWidget.h"
#include "Components/Button.h"
#include "Data/PerkDataTableRow.h"
#include "Subsystem/MVVMSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UPerkSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Add Viewport마다 랜덤한 3개의 퍽 데이터를 가져와서 버튼에 세팅.
	SetupPerkButtons();

	// MVVM Subsystem에 자신을 등록
	if (UMVVMSubsystem* mVVMSubsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UMVVMSubsystem>())
	{
		mVVMSubsystem->ResgisterPerkSelectionWidget(this);
	}

	if (PerkSelectButton1 && PerkSelectButton2 && PerkSelectButton3
        && SelectedPerk)
	{
		// 버튼 1에 아이콘 세팅
		FButtonStyle ButtonStyle;
		ButtonStyle.Normal.SetResourceObject(SelectedPerk->Icon);
		PerkSelectButton1->SetStyle(ButtonStyle);

		PerkSelectButton1->OnClicked.AddDynamic(this, &UPerkSelectionWidget::OnPerkSelectButtonClicked);
	}
}

void UPerkSelectionWidget::SetupPerkButtons()
{
	TArray<FPerkSelectDataTableRow> SelectedPerkRows = GetRandomPerkDataRows(PerkDataRow, 3);
	
	if (SelectedPerkRows.Num() > 0)
	{
		SelectedPerk = SelectedPerkRows[0].PerkDataAsset;
	}
}

//
// -- Efraimidis-Spirakis 알고르즘 --
// Score = Random(0, 1) ^ (1/Weight)으로 각 행에 대한 점수를 계산하여 상위 K를 선택
//
TArray<FPerkSelectDataTableRow> UPerkSelectionWidget::GetRandomPerkDataRows(UDataTable* InDataTable, int32 NumSelect)
{
	TArray<FPerkSelectDataTableRow> ResultRows;

	// 데이터 테이블의 SelectWeight를 기반으로 랜덤한 3개의 퍽 데이터를 선택하여 버튼에 세팅
	if (!InDataTable || NumSelect <= 0) return ResultRows;

	TArray<FPerkSelectDataTableRow*> AllRows;
	InDataTable->GetAllRows<FPerkSelectDataTableRow>(TEXT("Get PerkSelectDataTable"), AllRows);

	if (AllRows.IsEmpty()) return ResultRows;

	// 모든 행이 뽑고 싶은 개수 미만일 경우, 모든 행을 반환하고 종료
	if (AllRows.Num() < NumSelect)
	{
		for (const FPerkSelectDataTableRow* Row : AllRows)
		{
			ResultRows.Add(*Row);
			return ResultRows;
		}
	}

	// 점수 계산  [ Score = Random(0, 1) ^ (1/Weight) ]
	TArray<FWeightCandidate> Candidates;
	Candidates.Reserve(AllRows.Num());

	for (FPerkSelectDataTableRow* Row : AllRows)
	{
		// 잘못된 가중치는 무시
		if (Row->SelectWeight <= 0.0f) continue;
	
		float RandomValue = FMath::FRand();
		double Score = FMath::Pow(RandomValue, 1.0f / Row->SelectWeight);

		Candidates.Add({ Row, Score });
	}

	// 점수를 기준으로 정렬
	Candidates.Sort();

	// 상위 K개 선택
	int32 Count = FMath::Min(NumSelect, Candidates.Num());
	for (int32 i = 0; i < Count; i++)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Selected Perk: %s with Score: %f"), *Candidates[i].PerkRow->PerkDataAsset->GetName(), Candidates[i].Score);
		ResultRows.Add(*Candidates[i].PerkRow);
	}

	return ResultRows;
}

void UPerkSelectionWidget::OnPerkSelectButtonClicked()
{
	if (OnTryEquippedPerk.IsBound() && SelectedPerk)
	{
		OnTryEquippedPerk.Broadcast(SelectedPerk);
	}
}