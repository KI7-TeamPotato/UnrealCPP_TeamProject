// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Enemy/EnemyDamagePopupWidget.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

void UEnemyDamagePopupWidget::NativeConstruct()
{
    Super::NativeConstruct();

    SetVisibility(ESlateVisibility::Collapsed);

    // 애니메이션 완료 이벤트 바인딩
    // 델리게이트에 함수 바인딩
    DamagePopupAnimationFinishedEvent.BindDynamic(this, &UEnemyDamagePopupWidget::OnDamagePopupAnimationFinished);
    // 애니메이션에 델리게이트 연결
    BindToAnimationFinished(DamagePopupAnimation, DamagePopupAnimationFinishedEvent);
}

void UEnemyDamagePopupWidget::OnDamagePopupAnimationFinished()
{
    SetVisibility(ESlateVisibility::Collapsed);
    RequestReturnPoolDelegate.ExecuteIfBound();
}

void UEnemyDamagePopupWidget::SetActivateDamagePopup(float DamageAmount)
{
    // 데미지 텍스트 설정
    // Visible 상태로 변경 후 애니메이션 재생
    if (DamageText)
    {
        DamageText->SetText(FText::AsNumber(FMath::RoundToInt(DamageAmount)));
    }
    SetVisibility(ESlateVisibility::Visible);

    PlayAnimation(DamagePopupAnimation);
}



