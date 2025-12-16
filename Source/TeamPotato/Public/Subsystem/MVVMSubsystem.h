// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MVVMSubsystem.generated.h"

class UPlayerStatusViewModel;
class UPerkViewModel;

class UPlayerResource;
class UPerkComponent;
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

	// ==============================================================================
	// 컴포넌트 등록 및 해제 함수들
	// ==============================================================================
	
	/*
	// --- 캐릭터, 컴포넌트 등록 및 해제 함수 ---
	UFUNCTION()
	void RegisterPlayerResourceComp(class UPlayerResource* NewComp);
	UFUNCTION()
	void UnregisterPlayerResourceComp(class UPlayerResource* ExitingComp);
	*/

	// --- 퍽 뷰모델 관련 함수들 ---
	UFUNCTION()
	void RegisterPerkComp(class UPerkComponent* NewComp);
	UFUNCTION()
	void UnregisterPerkComp(class UPerkComponent* ExitingComp);

private:
	UPROPERTY()
	TObjectPtr<UPlayerStatusViewModel> PlayerStatusViewModel;

	UPROPERTY()
	TObjectPtr<UPerkViewModel> PerkViewModel;
};
