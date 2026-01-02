// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/BossBase.h"
#include "Blueprint/UserWidget.h"
#include "LastBoss.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameClear);

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API ALastBoss : public ABossBase
{
	GENERATED_BODY()
public:
    ALastBoss();

    UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
    FOnGameClear OnGameClear;


protected:
    virtual void BeginPlay() override;

    virtual void OnDie() override;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;

    // 14초 뒤에 호출될 함수
    void ShowVictoryWidget();

    // 타이머 핸들
    FTimerHandle VictoryWidgetTimerHandle;

    // 에디터에서 선택할 위젯 클래스 (BP_VictoryWidget 등)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> VictoryWidgetClass;

    // 생성된 위젯을 저장할 변수
    UPROPERTY()
    UUserWidget* VictoryWidget;
};

