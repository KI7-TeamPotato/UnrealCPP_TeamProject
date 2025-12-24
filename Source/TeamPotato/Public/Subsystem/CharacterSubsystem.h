// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/CharacterTypes.h"
#include "Data/CharacterDataTableRow.h"
#include "Data/WeaponDataAsset.h"
#include "CharacterSubsystem.generated.h"

// 델리게이트 선언: 선택된 캐릭터 타입이 변경되었을 때 알림
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectedCharacterChanged);

/**
 * 
 */
struct PlayerSaveData
{
public:
    // 초기 캐릭터는 전사로 설정
    ECharacterType SelectedCharacter = ECharacterType::Warrior;
    UWeaponDataAsset* EquippedMainWeapon = nullptr;
    UWeaponDataAsset* EquippedSubWeapon = nullptr;
    UTexture2D* PlayerIcon = nullptr;
    UTexture2D* PlayerIllustration = nullptr;
};

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

    // 현재 플레이어 타입의 초기 상태로 리셋
    UFUNCTION(BlueprintCallable, Category = "Character")
    void ResetPlayerDataToInitialState();

	// =============================================================
	// 진행 중의 플레이어 정보
	// =============================================================

	// 선택된 캐릭터 타입 설정
	UFUNCTION(BlueprintCallable, Category = "Character")
	void SetSelectedCharacterType(ECharacterType NewCharacterType);

    // 장착 중인 메인 무기 설정
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SetEquippedMainWeapon(UWeaponDataAsset* NewMainWeapon);

    // 장착 중인 서브 무기 설정
    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void SetEquippedSubWeapon(UWeaponDataAsset* NewSubWeapon);

    // 플레이어 아이콘 설정
    UFUNCTION(BlueprintCallable, Category = "Character")
    void SetPlayerIcon(UTexture2D* NewIcon) { CurrentPlayerData.PlayerIcon = NewIcon; }

    // 플레이어 일러스트 설정
    UFUNCTION(BlueprintCallable, Category = "Character")
    void SetPlayerIllustration(UTexture2D* NewIllust) { CurrentPlayerData.PlayerIllustration = NewIllust; }

	// 선택된 캐릭터 타입 반환
	UFUNCTION(BlueprintPure, Category = "Character")
	ECharacterType GetSelectedCharacterType() const { return CurrentPlayerData.SelectedCharacter; }

    // 장착 중인 메인 무기 반환
    UFUNCTION(BlueprintPure, Category = "Weapon")
    UWeaponDataAsset* GetEquippedMainWeapon() const { return CurrentPlayerData.EquippedMainWeapon; }

    // 장착 중인 서브 무기 반환
    UFUNCTION(BlueprintPure, Category = "Weapon")
    UWeaponDataAsset* GetEquippedSubWeapon() const { return CurrentPlayerData.EquippedSubWeapon; }

    // 플레이어 아이콘 반환
    UFUNCTION(BlueprintPure, Category = "Character")
    UTexture2D* GetPlayerIcon() const { return CurrentPlayerData.PlayerIcon; }

    // 플레이어 일러스트 반환
    UFUNCTION(BlueprintPure, Category = "Character")
    UTexture2D* GetPlayerIllustration() const { return CurrentPlayerData.PlayerIllustration; }

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
    //=============================================================
    // 기본 캐릭터의 데이터(로비 진입 시(캐릭터 상태 초기화)시에만 사용)
    //=============================================================
	
    // 캐릭터 데이터 테이틀
	UPROPERTY()
	TObjectPtr<UDataTable> CharacterDataTable = nullptr;

	// 캐릭터 타입별 데이터 캐시(빠른 검색)
	UPROPERTY()
	TMap<ECharacterType, FCharacterDataTableRow> CharacterDataCache;

    //=============================================================
    // 진행 중의 플레이어 정보
    //=============================================================

	// 이번 게임의 선택된 캐릭터 정보(로비로 이동하면 캐릭터 타입을 제외하고 전부 초기화, 이후 챕터 진행에서는 저장)
    PlayerSaveData CurrentPlayerData;
};
