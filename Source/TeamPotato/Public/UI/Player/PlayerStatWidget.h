// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatWidget.generated.h"

/**
 * 
 */
class UProgressBar;
class UImage;
class UPlayerStatusViewModel;

UCLASS()
class TEAMPOTATO_API UPlayerStatWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // 외부에서 ViewModel 주입
    UFUNCTION(BlueprintCallable, Category = "MVVM")
    void SetViewModel(UPlayerStatusViewModel* InViewModel);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
    void BindViewModel();
    void UnbindViewModel();

	/// --- 뷰모델 바인딩 함수 ---
	UFUNCTION()
	void SetPlayerHealthBar(float NewHealthPercent);

	UFUNCTION()
	void SetPlayerIcon(UTexture2D* NewPlayerIcon);

///==================== 변수 ======================== 

protected:
	// --- 플레이어의 체력 프로그레스바---
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UProgressBar> PlayerHealthBar;

	// --- 플레이어의 아이콘 이미지 ---
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> PlayerIconImage;

private:
    // --- 플레이어 상태 뷰모델 ---
	UPROPERTY()
	TObjectPtr<UPlayerStatusViewModel> PlayerStatusViewModel;

    // 중복 바인딩 방지 플래그
    bool bIsViewModelBound = false;
};
