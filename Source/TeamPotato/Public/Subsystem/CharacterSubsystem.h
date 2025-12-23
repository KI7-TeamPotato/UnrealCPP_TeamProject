// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/CharacterTypes.h"
#include "Data/CharacterDataTableRow.h"
#include "CharacterSubsystem.generated.h"

// 델리게이트 선언: 선택된 캐릭터 타입이 변경되었을 때 알림
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedCharacterChanged, ECharacterType, NewCharacterType);

/**
 * 
 */

// ==============================================================
// 플레이어 정보를 통합하여 관리하는 서브시스템
// ==============================================================
UCLASS()
class TEAMPOTATO_API UCharacterSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// =============================================================
	// 데이터 테이블
	// =============================================================
	
	// 데이터 테이블 반환
	UFUNCTION(BlueprintPure, Category = "Character Subsystem|Character")
	UDataTable* GetCharacterDataTable() const { return CharacterDataTable; }

	// 특정 캐릭터 타입의 데이터 반환
	UFUNCTION(BlueprintPure, Category = "Character Subsystem|Character")
	bool GetCharacterData(ECharacterType CharacterType, FCharacterDataTableRow& OutData) const;

	//모든 캐릭터 데이터 반환
	const TMap<ECharacterType, FCharacterDataTableRow>& GetAllCharacterDataMap() const { return CharacterDataCache; }

	// =============================================================
	// 선택된 캐릭터
	// =============================================================

	// 선택된 캐릭터 타입 설정
	UFUNCTION(BlueprintCallable, Category = "Character")
	void SetSelectedCharacterType(ECharacterType NewCharacterType);

	// 선택된 캐릭터 타입 반환
	UFUNCTION(BlueprintPure, Category = "Character")
	ECharacterType GetSelectedCharacterType() const { return SelectedCharacterType; }

	// 선택된 캐릭터의 데이터 반환
	UFUNCTION(BlueprintPure, Category = "Character")
	bool GetSelectedCharacterData(FCharacterDataTableRow& OutData) const;

	// =============================================================
	// 델리게이트
	// =============================================================
	UPROPERTY(BlueprintAssignable, Category = "Character Subsystem|Character")
	FOnSelectedCharacterChanged OnSelectedCharacterChanged;

private:
	// 데이터 테이블 로드
	void LoadAndCacheCharacterData();

    // 선택된 캐릭터 타입으로 빙의
    void PossessSelectedCharacterType();
    
// ===================================================================================
// 변수
// ===================================================================================
private:
	// 캐릭터 데이터 테이틀
	UPROPERTY()
	TObjectPtr<UDataTable> CharacterDataTable = nullptr;

	// 캐릭터 타입별 데이터 캐시(빠른 검색)
	UPROPERTY()
	TMap<ECharacterType, FCharacterDataTableRow> CharacterDataCache;

	// 현재 선택된 캐릭터
	UPROPERTY()
	ECharacterType SelectedCharacterType = ECharacterType::None;
};
