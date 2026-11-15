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

    UFUNCTION(BlueprintCallable, Category = "Boss|Beam")
    void BeginBeamSweep(AActor* Beam);

    UFUNCTION(BlueprintCallable, Category = "Boss|Beam")
    void UpdateBeamSweep(float PlaybackSeconds);

    UFUNCTION(BlueprintCallable, Category = "Boss|Beam")
    void EndBeamSweep();

private:
    UPROPERTY(EditDefaultsOnly, Category = "Boss|Beam", meta = (Units = "deg/s"))
    float BodyYawDegreesPerSecond = 18.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Boss|Beam", meta = (Units = "deg/s"))
    float BeamYawDegreesPerSecond = -48.0f;

    TWeakObjectPtr<AActor> SweepingBeam;
    TOptional<FQuat> SweepStartBodyRotation;
    FQuat SweepStartBeamRotation = FQuat::Identity;

public:

    UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
    FOnGameClear OnGameClear;

    virtual void ExecutePattern(int32 PatternIndex) override;
    virtual void FinishCurrentPattern() override;
    void CancelActivePattern();

    UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
    void TrackPatternMontage(UAnimMontage* Montage);

    UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
    void TrackPatternHazard(AActor* Hazard);

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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

private:
    void StopPattern(bool bEndEncounter);
    uint64 PatternSerial = 0;
    uint64 ActivePattern = 0;
    int32 PatternMontageInstance = INDEX_NONE;
    TMap<TWeakObjectPtr<AActor>, uint64> PatternHazards;
};

