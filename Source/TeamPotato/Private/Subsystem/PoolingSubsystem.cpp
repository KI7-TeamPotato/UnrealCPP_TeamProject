// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/PoolingSubsystem.h"
#include "Common/MyGameSettings.h"
#include "Enemy/DamagePopup/EnemyDamagePopupActor.h"

void UPoolingSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // 데미지 팝업 위젯 풀 초기화
    InitializeDamagePopupActorPool();
}

void UPoolingSubsystem::Deinitialize()
{

    Super::Deinitialize();
}

void UPoolingSubsystem::InitializeDamagePopupActorPool()
{
    // 데미지 팝업 액터 풀링 배열 초기화
    AvailableDamagePopupActors.Reserve(DamagePopupActorPoolSize);

    // 데미지 팝업 클래스 가져오기
    UMyGameSettings* GameSettings = UMyGameSettings::Get();
    TSubclassOf<AEnemyDamagePopupActor> DamagePopupWidgetClass = GameSettings->EnemyDamagePopupActorClass.LoadSynchronous();

    // 데미지 팝업 액터 풀링 초기화
    for (int32 i = 0; i < DamagePopupActorPoolSize; i++)
    {
        AEnemyDamagePopupActor* DamagePopupActor 
            = GetWorld()->SpawnActor<AEnemyDamagePopupActor>(DamagePopupWidgetClass);

        if (DamagePopupActor)
        {
            // 보이지 않게 설정하고 사용 가능 배열에 추가
            AvailableDamagePopupActors.Add(DamagePopupActor);
        }
    }
}