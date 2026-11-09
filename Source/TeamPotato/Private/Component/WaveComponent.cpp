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
	ResetWaveSystem();
	SpawnPoints = InSpawnPoints;
	SpawnWave(0); // 첫 웨이브 시작
}

void UWaveComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ResetWaveSystem();
    Super::EndPlay(EndPlayReason);
}

void UWaveComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
    ResetWaveSystem();
    Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UWaveComponent::ClearWarningEffects()
{
    for (const TWeakObjectPtr<AActor>& Effect : WarningEffects)
        if (Effect.IsValid()) Effect->Destroy();
    WarningEffects.Reset();
}

void UWaveComponent::ResetWaveSystem()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(SpawnTimerHandle);
        World->GetTimerManager().ClearTimer(WaveTimerHandle);
    }
    bWaitingForSpawn = false;
    ClearWarningEffects();
    for (const TWeakObjectPtr<AEnemyCharacter>& Enemy : WaveEnemies)
        if (Enemy.IsValid()) Enemy->OnDeath.RemoveDynamic(this, &UWaveComponent::OnEnemyKilled);
    WaveEnemies.Reset();
    RemainingEnemies = 0;
}

bool UWaveComponent::GetSpawnTransforms(const FSpawnInfo& Info, FTransform& SpawnTransform, FTransform& EffectTransform) const
{
    if (!Info.EnemyClass || Info.EnemyClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists) ||
        !SpawnPoints.IsValidIndex(Info.SpawnPointIndex) || !IsValid(SpawnPoints[Info.SpawnPointIndex])) return false;
    SpawnTransform = SpawnPoints[Info.SpawnPointIndex]->GetComponentTransform();
    EffectTransform = SpawnTransform;
    // Retain the floor placement used by the existing room spawn effect.
    FVector EffectLocation = SpawnTransform.GetLocation();
    EffectLocation.Z = 0.0f;
    EffectTransform.SetLocation(EffectLocation);
    return true;
}

void UWaveComponent::SpawnWave(int32 WaveIndex)
{
    if (!GetWorld()) return;
    ClearWarningEffects();
    CurrentWaveIndex = WaveIndex;
    //웨이브 인덱스가 유효한지 먼저 검사
    if (!Waves.IsValidIndex(WaveIndex))
    {
        OnAllWavesCleared.Broadcast();
        return;
    }

    const TSubclassOf<AActor> WarningClass = EnemyWarningEffect ? EnemyWarningEffect : EnemyEffect;
    for (const FSpawnInfo& Info : Waves[WaveIndex].SpawnList)
    {
        FTransform SpawnTransform, EffectTransform;
        if (!GetSpawnTransforms(Info, SpawnTransform, EffectTransform) || !WarningClass) continue;
        FActorSpawnParameters Params;
        Params.Owner = GetOwner();
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        if (AActor* Warning = GetWorld()->SpawnActor<AActor>(WarningClass, EffectTransform, Params))
            WarningEffects.Add(Warning);
    }

    bWaitingForSpawn = true;
    FTimerDelegate TimerDel;

    TimerDel.BindUObject(this, &UWaveComponent::SpawnEnemiesDelayed, WaveIndex);

    // Warning first, then monster and arrival effect one second later.
    GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, TimerDel, 1.0f, false);
}

void UWaveComponent::OnEnemyKilled()
{
    if (bWaitingForSpawn || RemainingEnemies <= 0) return;
    RemainingEnemies--;
    UE_LOG(LogTemp, Log, TEXT("%d"), RemainingEnemies);
    if (RemainingEnemies == 0) ScheduleNextWave();
}

void UWaveComponent::ScheduleNextWave()
{
    if (!GetWorld() || GetWorld()->GetTimerManager().IsTimerActive(WaveTimerHandle)) return;
    FTimerDelegate NextWave;
    NextWave.BindUObject(this, &UWaveComponent::SpawnWave, CurrentWaveIndex + 1);
    GetWorld()->GetTimerManager().SetTimer(WaveTimerHandle, NextWave, 3.0f, false);
}

void UWaveComponent::SpawnEnemiesDelayed(int32 WaveIndex)
{
    if (!GetWorld() || !bWaitingForSpawn) return;
    bWaitingForSpawn = false;
    ClearWarningEffects();
    CurrentWaveIndex = WaveIndex;
    RemainingEnemies = 0; // 적 숫자 초기화

    // 웨이브 데이터 가져오기
    if (!Waves.IsValidIndex(WaveIndex)) return;
    const FWaveData& CurrentWave = Waves[WaveIndex];

    for (const FSpawnInfo& Info : CurrentWave.SpawnList)
    {
        FTransform SpawnTransform, EffectTransform;
        if (GetSpawnTransforms(Info, SpawnTransform, EffectTransform))
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = GetOwner();
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            AEnemyCharacter* NewEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(Info.EnemyClass, SpawnTransform, SpawnParams);
            if (IsValid(NewEnemy))
            {
                NewEnemy->OnDeath.AddDynamic(this, &UWaveComponent::OnEnemyKilled);
                WaveEnemies.Add(NewEnemy);
                RemainingEnemies++;
                if (EnemyEffect) GetWorld()->SpawnActor<AActor>(EnemyEffect, EffectTransform, SpawnParams);
            }
        }
    }

    // 만약 데이터가 비어서 적이 0명이면 바로 다음 단계 처리
    if (RemainingEnemies == 0)
    {
        ScheduleNextWave();
    }
}


