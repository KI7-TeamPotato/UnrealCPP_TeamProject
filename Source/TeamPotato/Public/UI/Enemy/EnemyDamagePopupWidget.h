// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyDamagePopupWidget.generated.h"

DECLARE_DYNAMIC_DELEGATE(FRequestReturnPool);

class UTextBlock;
class UWidgetAnimation;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UEnemyDamagePopupWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION()
    void SetActivateDamagePopup(float DamageAmount);

    UFUNCTION()
    UWidgetAnimation* GetDamagePopupAnimation() const { return DamagePopupAnimation; }

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void OnDamagePopupAnimationFinished();

public:
    UPROPERTY()
    FWidgetAnimationDynamicEvent DamagePopupAnimationFinishedEvent;

    UPROPERTY()
    FRequestReturnPool RequestReturnPoolDelegate;
protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> DamageText = nullptr;

    // 직렬화에서 제외되는 애니메이션 프로퍼티(런타임에만 사용)
    UPROPERTY(Transient, meta = (BindWidgetAnim))
    TObjectPtr<UWidgetAnimation> DamagePopupAnimation = nullptr;

};
