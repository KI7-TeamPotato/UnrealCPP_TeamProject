// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonRoom1.h"
#include "Components/ArrowComponent.h"



ADungeonRoom1::ADungeonRoom1()
{
	WaveSystemComp = CreateDefaultSubobject<UWaveComponent>(TEXT("WaveSystemComp"));

	ExitArrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow1"));
	ExitArrow2 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow2"));
	ExitArrow3 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow3"));

	//SpawnPoint1 = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPoint1"));
	SpawnPoint2 = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPoint2"));
	SpawnPoint3 = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPoint3"));

	ExitArrow1->SetupAttachment(ExitPointsFolder);
	ExitArrow2->SetupAttachment(ExitPointsFolder);
	ExitArrow3->SetupAttachment(ExitPointsFolder);

	//SpawnPoint1->SetupAttachment(SpawnPointsFolder);
	SpawnPoint2->SetupAttachment(SpawnPointsFolder);
	SpawnPoint3->SetupAttachment(SpawnPointsFolder);

	//SpawnPoint1->ArrowColor = FColor::Purple;
	SpawnPoint2->ArrowColor = FColor::Purple;
	SpawnPoint3->ArrowColor = FColor::Purple;
}

void ADungeonRoom1::BeginPlay()
{
	Super::BeginPlay();

	// 3. 컴포넌트에게 스폰 포인트 위치 정보 전달 및 시스템 가동
	TArray<UArrowComponent*> Points;
	//Points.Add(SpawnPoint1);
	Points.Add(SpawnPoint2);
	Points.Add(SpawnPoint3);

	// 플레이어가 방에 들어오면 시작하는 로직이 있다면 거기서 호출해도 됨
	WaveSystemComp->StartWaveSystem(Points);

	// 4. 전투 끝나면 문 열기 등의 로직 연결
	WaveSystemComp->OnAllWavesCleared.AddDynamic(this, &ADungeonRoom1::HandleBattleEnd);
}

void ADungeonRoom1::HandleBattleEnd()
{
	UE_LOG(LogTemp, Log, TEXT("All Waves Cleared! Open the Doors!"));
	// 문 열기 로직
}
