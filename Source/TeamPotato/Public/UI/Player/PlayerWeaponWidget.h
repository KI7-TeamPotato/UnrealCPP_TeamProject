// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWeaponWidget.generated.h"

class UTextBlock;
class UImage;
class UProgressBar;
class UWeaponViewModel;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UPlayerWeaponWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintCallable, Category = "MVVM")
    void SetViewModel(UWeaponViewModel* InViewModel);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

private:
    void BindViewModel();
    void UnbindViewModel();

    // --- 뷰모델 바인딩 함수 ---
    // 플레이어에게 있는 무기 공격 가능 자원 변경 함수
    UFUNCTION()
    void UpdatePlayerResourceBar(float NewResourcePercent);

    // 플레이어의 메인 무기 정보 변경 함수 (데이터 에셋 혹은 테이블도 생각)
     UFUNCTION()
    void UpdateMainWeaponInfo(FText InWeaponName, UTexture2D* InWeaponIcon);

    UFUNCTION()
    void UpdateSubWeaponInfo(UTexture2D* InSubWeaponIcon);

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> WeaponIconImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> WeaponName;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> SubWeaponIconImage;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> PlayerResourceBar;

private:
    UPROPERTY()
    TObjectPtr<UWeaponViewModel> WeaponViewModel;

    bool bIsViewModelBound = false;
};
