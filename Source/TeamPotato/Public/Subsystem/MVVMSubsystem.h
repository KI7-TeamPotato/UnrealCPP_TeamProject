// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MVVMSubsystem.generated.h"

class UPlayerStatusViewModel;
class UPerkViewModel;
class UWeaponViewModel;
class MinimapViewModel;

class UPlayerResource;
class UPerkComponent;
class UWeaponComponent;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UMVVMSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// --- 뷰모델 반환 함수 ---
	UFUNCTION(BlueprintPure)
	UPlayerStatusViewModel* GetPlayerStatusViewModel();
	UFUNCTION(BlueprintPure)
    UPerkViewModel* GetPerkViewModel();
    UFUNCTION(BlueprintPure)
    UWeaponViewModel* GetWeaponViewModel();
    UFUNCTION(BlueprintPure)
    UMinimapViewModel* GetMinimapViewModel();
	// ==============================================================================
	// 컴포넌트 등록 및 해제 함수들
	// ==============================================================================
	
	// --- 캐릭터, 컴포넌트 등록 및 해제 함수 ---
	UFUNCTION()
	void RegisterPlayerResourceComp(class UPlayerResource* NewComp);
	UFUNCTION()
	void UnregisterPlayerResourceComp(class UPlayerResource* ExitingComp);
	

	// --- 퍽 컴포넌트 등록 및 해제 함수 ---
	UFUNCTION()
	void RegisterPerkComp(class UPerkComponent* NewComp);
	UFUNCTION()
	void UnregisterPerkComp(class UPerkComponent* ExitingComp);

    // --- 무기 컴포넌트 등록 및 해제 함수 ---
    UFUNCTION()
    void RegisterWeaponComp(class UWeaponComponent* NewComp);
    UFUNCTION()
    void UnregisterWeaponComp(class UWeaponComponent* ExitingComp);

    // --- DungeonGenerator 액터 등록 및 해제 함수 ---
    UFUNCTION()
    void RegisterDungeonGeneratorActor(class ADungeonGanarator* NewActor);
    UFUNCTION()
    void UnregisterDungeonGeneratorActor(class ADungeonGanarator* ExitingActor);

private:
	UPROPERTY()
	TObjectPtr<UPlayerStatusViewModel> PlayerStatusViewModel;

	UPROPERTY()
	TObjectPtr<UPerkViewModel> PerkViewModel;

    UPROPERTY()
    TObjectPtr<UWeaponViewModel> WeaponViewModel;

    UPROPERTY()
    TObjectPtr<UMinimapViewModel> MinimapViewModel;
};
