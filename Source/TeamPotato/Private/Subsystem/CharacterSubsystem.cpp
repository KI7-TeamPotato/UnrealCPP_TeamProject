// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/CharacterSubsystem.h"
#include "Common/MyGameSettings.h"
#include "Player/TestCharacter.h"
#include "Component/WeaponComponent.h"

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

void UCharacterSubsystem::ResetPlayerDataToInitialState()
{
    CurrentPlayerData.EquippedMainWeapon = CharacterDataCache.Find(CurrentPlayerData.SelectedCharacter)->DefaultWeaponDataAsset;
    CurrentPlayerData.EquippedSubWeapon = nullptr;
}

// =============================================================
// 선택된 캐릭터
// =============================================================

// 선택된 캐릭터 타입 설정
void UCharacterSubsystem::SetSelectedCharacterType(ECharacterType NewCharacterType)
{
	if (CurrentPlayerData.SelectedCharacter != NewCharacterType)
	{
        // 캐릭터 변경시에 기본 캐릭터 상태로 초기화
        CurrentPlayerData.SelectedCharacter = NewCharacterType;
        CurrentPlayerData.EquippedMainWeapon = CharacterDataCache.Find(NewCharacterType)->DefaultWeaponDataAsset;
        CurrentPlayerData.EquippedSubWeapon = nullptr;

        // 선택된 캐릭터 타입으로 빙의
        PossessSelectedCharacterType();

		// 실제 캐릭터의 변경된 데이터 처리는 이 델리게이트를 구독한 곳에서 처리
		OnSelectedCharacterChanged.Broadcast(NewCharacterType);

		UE_LOG(LogTemp, Log, TEXT("UCharacterSubsystem::SetSelectedCharacterType - Changed to %d"), static_cast<int32>(NewCharacterType));
	}
}

void UCharacterSubsystem::SetEquippedMainWeapon(UWeaponDataAsset* NewMainWeapon)
{
    if (CurrentPlayerData.EquippedMainWeapon != NewMainWeapon)
    {
        CurrentPlayerData.EquippedMainWeapon = NewMainWeapon;
    }
}

void UCharacterSubsystem::SetEquippedSubWeapon(UWeaponDataAsset* NewSubWeapon)
{
    if (CurrentPlayerData.EquippedSubWeapon != NewSubWeapon)
    {
        CurrentPlayerData.EquippedSubWeapon = NewSubWeapon;
    }
}

bool UCharacterSubsystem::GetSelectedCharacterData(FCharacterDataTableRow& OutData) const
{
    return GetCharacterData(CurrentPlayerData.SelectedCharacter, OutData);
}

// 선택된 캐릭터 타입으로 빙의
void UCharacterSubsystem::PossessSelectedCharacterType()
{
    // 현재 플레이어 폰의 위치와 회전을 가져옴
    APawn* CurrentPawn = GetWorld()->GetFirstPlayerController()->GetPawn();

    FVector SpawnLocation = CurrentPawn->GetActorLocation();
    FRotator SpawnRotation = CurrentPawn->GetActorRotation();

    // 현재 플레이어 폰을 제거
    CurrentPawn->Destroy();

    // 새로운 캐릭터 타입에 해당하는 폰을 스폰
    TSubclassOf<ATestCharacter> SpawnCharacter = CharacterDataCache.Find(CurrentPlayerData.SelectedCharacter)->CharacterClass;

    ATestCharacter* NewPawn = GetWorld()->SpawnActor<ATestCharacter>(SpawnCharacter, SpawnLocation, SpawnRotation);
    if (NewPawn)
    {
        GetWorld()->GetFirstPlayerController()->Possess(NewPawn);

        // 플레이어 beginPlay에서 처리
        //NewPawn->GetWeaponComponent()->PickupWeapon(CharacterDataCache.Find(CurrentPlayerData.SelectedCharacter)->DefaultWeaponDataAsset);
    }
}


