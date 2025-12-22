// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/WaveComponent.h"
#include "TimerManager.h"

// Sets default values for this component's properties
UWaveComponent::UWaveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UWaveComponent::BeginPlay()
{
	Super::BeginPlay();

}


void UWaveComponent::StartWaveSystem(const TArray<UArrowComponent*>& InSpawnPoints)
{
	SpawnPoints = InSpawnPoints;
	SpawnWave(0); // 첫 웨이브 시작
}

void UWaveComponent::SpawnWave(int32 WaveIndex)
{
    //웨이브 인덱스가 유효한지 먼저 검사
    if (!Waves.IsValidIndex(WaveIndex))
    {
        OnAllWavesCleared.Broadcast();
        return;
    }

    //2초 뒤에 SpawnEnemiesDelayed 함수를 실행하도록 타이머 설정
    FTimerDelegate TimerDel;

    TimerDel.BindUObject(this, &UWaveComponent::SpawnEnemiesDelayed, WaveIndex);

    // 2초 뒤에 실행
    GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, TimerDel, 2.0f, false);
}

void UWaveComponent::OnEnemyKilled()
{
    RemainingEnemies--;
    UE_LOG(LogTemp, Log, TEXT("%d"), RemainingEnemies);
    if (RemainingEnemies <= 0)
    {
        // 3초 뒤 다음 웨이브
        GetWorld()->GetTimerManager().SetTimer(WaveTimerHandle, [this]()
            {
                SpawnWave(CurrentWaveIndex + 1);
            }, 3.0f, false);
    }
}

void UWaveComponent::SpawnEnemiesDelayed(int32 WaveIndex)
{
    CurrentWaveIndex = WaveIndex;
    RemainingEnemies = 0; // 적 숫자 초기화

    // 웨이브 데이터 가져오기
    if (!Waves.IsValidIndex(WaveIndex)) return; // 안전 검사 한 번 더
    const FWaveData& CurrentWave = Waves[WaveIndex];

    for (const FSpawnInfo& Info : CurrentWave.SpawnList)
    {
        if (Info.EnemyClass && SpawnPoints.IsValidIndex(Info.SpawnPointIndex))
        {
            UArrowComponent* TargetPoint = SpawnPoints[Info.SpawnPointIndex];
            if (TargetPoint)
            {
                FTransform SpawnTransform = TargetPoint->GetComponentTransform();
                FTransform EffectTransform = TargetPoint->GetComponentTransform();
                EffectTransform.SetLocation(FVector(SpawnTransform.GetLocation().X, SpawnTransform.GetLocation().Y, 0.0f));
                FActorSpawnParameters SpawnParams;
                SpawnParams.Owner = GetOwner();
                SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                // 몬스터 생성
                AEnemyCharacter* NewEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(Info.EnemyClass, SpawnTransform, SpawnParams);
                AActor* effect = GetWorld()->SpawnActor<AActor>(EnemyEffect, EffectTransform, SpawnParams);

                if (NewEnemy)
                {
                    NewEnemy->OnDeath.AddDynamic(this, &UWaveComponent::OnEnemyKilled);
                    RemainingEnemies++;
                }
            }
        }
    }

    // 만약 데이터가 비어서 적이 0명이면 바로 다음 단계 처리
    if (RemainingEnemies == 0)
    {
        OnEnemyKilled();
    }
}


