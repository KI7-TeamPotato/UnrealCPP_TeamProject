// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "Components/ArrowComponent.h"
#include "WaveComponent.generated.h"

USTRUCT(BlueprintType)
struct FSpawnInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AEnemyCharacter> EnemyClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SpawnPointIndex = 0;
};

USTRUCT(BlueprintType)
struct FWaveData
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (TitleProperty = "EnemyClass"))
    TArray<FSpawnInfo> SpawnList;
};

// 델리게이트: 모든 웨이브가 끝났을 때 방에게 알려줌
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllWavesCleared);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMPOTATO_API UWaveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWaveComponent();
protected:
    virtual void BeginPlay() override;

public:
    // 웨이브 데이터 
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wave System")
    TArray<FWaveData> Waves;

    // 현재 남은 적
    UPROPERTY(VisibleAnywhere, Category = "Wave System")
    int32 RemainingEnemies = 0;

    // 현재 웨이브 인덱스
    int32 CurrentWaveIndex = 0;

    // 외부(방)에서 스폰 포인트 위치들을 넣어줘야 함
    TArray<UArrowComponent*> SpawnPoints;

    // 방에서 호출할 함수: 시스템 시작
    UFUNCTION(BlueprintCallable)
    void StartWaveSystem(const TArray<UArrowComponent*>& InSpawnPoints);

    // 웨이브 클리어 알림
    UPROPERTY(BlueprintAssignable)
    FOnAllWavesCleared OnAllWavesCleared;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<AActor> EnemyEffect;
private:
    void SpawnWave(int32 WaveIndex);

    UFUNCTION()
    void OnEnemyKilled();

    void SpawnEnemiesDelayed(int32 WaveIndex);

    FTimerHandle WaveTimerHandle;

    FTimerHandle SpawnTimerHandle;
};
