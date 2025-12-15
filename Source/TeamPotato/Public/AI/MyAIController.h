// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "Data/EnumBase.h"
#include "MyAIController.generated.h"

class UAISenseConfig_Sight;
class UAISenseConfig_Damage;
/**
 * 
 */
UCLASS()
class TEAMPOTATO_API AMyAIController : public AAIController
{
	GENERATED_BODY()
public:
    AMyAIController();
protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;

public:
    //인공지능 퍼펙션(감각)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    class UAIPerceptionComponent* AIPerceptionComp;

    //시야 감지
    class UAISenseConfig_Sight* SightConfig;
    //데미지 들어오는거 감지
    class UAISenseConfig_Damage* DamageConfig;

    // 블랙보드 키 이름 (오타 방지용)
    const FName Key_State = FName("State");
    const FName Key_Target = FName("AttackTarget"); // 스크린샷의 AttackTarget

    //현재 상태 반환
    EEnemyState GetCurrentState() const;

    //공격 상태로 바꾸는 함수(아직은 다시 순찰 모드로 들어갈 계획 없음)
    void SetStateAsAttack(AActor* TargetActor);

    //아무거나 감각이 바뀌면(무언가를 감지하면) 실행
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
