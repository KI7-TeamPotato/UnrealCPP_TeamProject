// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PerkCardWidget.generated.h"

/**
 * 
 */
class UImage;
class UTextBlock;
class UButton;
class UPerkDataAsset;
class UBorder;

// 퍽 카드 선택 시 브로드캐스트할 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerkCardSelected, UPerkDataAsset*, SelectedPerk);

UCLASS()
class TEAMPOTATO_API UPerkCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION()
    void SetupPerkCard(UPerkDataAsset* InPerkDataAsset);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

private:
    UFUNCTION()
    void OnPerkSelectButtonClicked();
    
    UFUNCTION()
    void OnPerkButtonHovered();

    UFUNCTION()
    void OnPerkButtonUnhovered();

public:
    UPROPERTY(BlueprintAssignable, Category = "Perk")
    FOnPerkCardSelected OnPerkCardSelected;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> SelectButton  = nullptr;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UBorder> BackgroundImage = nullptr;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UImage> PerkImage = nullptr;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> PerkName = nullptr;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> PerkDescript = nullptr;

    // --- 마우스 커서를 호버 및 언호버시의 배경 색상 ---
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perk|Color")
    FLinearColor DefaultBackgroundColor = FLinearColor(0.7f, 0.7f, 0.7f, 1.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perk|Color")
    FLinearColor HoveredBackgroundColor = FLinearColor(1.f, 1.f, 1.f, 1.f);

private:
    // --- 퍽 데이터를 가지고 있다가 OnClicked 이벤트 발생 시 퍽 컴포넌트에 전달 ---
    UPROPERTY(VisibleAnywhere, Category = "Perk", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UPerkDataAsset> PerkDataAsset = nullptr;
};
