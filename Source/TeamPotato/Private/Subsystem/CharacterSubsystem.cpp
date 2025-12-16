// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/CharacterSubsystem.h"
#include "Common/MyGameSettings.h"

void UCharacterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Warning, TEXT("UCharacterSubsystem::Initialize"));

	LoadAndCacheCharacterData();
}

void UCharacterSubsystem::Deinitialize()
{
	UE_LOG(LogTemp, Warning, TEXT("UCharacterSubsystem::Deinitialized"));

	CharacterDataTable = nullptr;
	CharacterDataCache.Empty();

	Super::Deinitialize();
}

void UCharacterSubsystem::LoadAndCacheCharacterData()
{
	UMyGameSettings* GameSettings = UMyGameSettings::Get();
	if (!GameSettings)
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterSubsystem::GetCharacterData - GameSettings is null"));
		return;
	}

	if (GameSettings->CharacterDataTable.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("UCharacterSubsystem::GetCharacterData - CharacterDataTable is null"));
		return;
	}

	CharacterDataTable = GameSettings->CharacterDataTable.LoadSynchronous();

	if (!CharacterDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterSubsystem::GetCharacterData - Failed to load CharacterDataTable"));
		return;
	}

	// 데이터 테이블을 TMap으로 캐싱
	TArray<FName> RowNames = CharacterDataTable->GetRowNames();

	for (const FName& RowName : RowNames)
	{
		FCharacterDataTableRow* RowData = CharacterDataTable->FindRow<FCharacterDataTableRow>(RowName, TEXT("UCharacterSubsystem::GetCharacterData"));
		if (RowData)
		{
			CharacterDataCache.Add(RowData->CharacterType, *RowData);
		}
	}
}

// =============================================================
// 데이터 테이블
// =============================================================

// 특정 캐릭터의 행 데이터를 OutData에 담아서 반환
bool UCharacterSubsystem::GetCharacterData(ECharacterType CharacterType, FCharacterDataTableRow& OutData) const
{
	const FCharacterDataTableRow* FoundData = CharacterDataCache.Find(CharacterType);
	if (FoundData)
	{
		OutData = *FoundData;
		return true;
	}
	return false;
}

// =============================================================
// 선택된 캐릭터
// =============================================================

// 선택된 캐릭터 타입 설정
void UCharacterSubsystem::SetSelectedCharacterType(ECharacterType NewCharacterType)
{
	if (SelectedCharacterType != NewCharacterType)
	{
		SelectedCharacterType = NewCharacterType;

		// 실제 캐릭터의 변경된 데이터 처리는 이 델리게이트를 구독한 곳에서 처리
		OnSelectedCharacterChanged.Broadcast(NewCharacterType);

		UE_LOG(LogTemp, Log, TEXT("UCharacterSubsystem::SetSelectedCharacterType - Changed to %d"), static_cast<int32>(NewCharacterType));
	}
}

bool UCharacterSubsystem::GetSelectedCharacterData(FCharacterDataTableRow& OutData) const
{
	return GetCharacterData(SelectedCharacterType, OutData);
}


