// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonRoom7.h"
#include "TeamPotato/Logic/DungeonGanarator.h"
#include "Item/Weapon/WeaponBoxActor.h"

ADungeonRoom7::ADungeonRoom7()
{
    TreasureBoxSpawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("TreasureBoxSpawnPoint"));
	ExitArrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow1"));
	ExitArrow2 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow2"));
	ClosingWall1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClosingWall1"));
	ClosingWall2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClosingWall2"));


    TreasureBoxSpawnPoint->SetupAttachment(OtherArchitecture);
	ExitArrow1->SetupAttachment(ExitPointsFolder);
	ExitArrow2->SetupAttachment(ExitPointsFolder);
	ClosingWall1->SetupAttachment(GeometryFolder);
	ClosingWall2->SetupAttachment(GeometryFolder);
}

void ADungeonRoom7::SpawnTreasureBox()
{
    TSubclassOf<AWeaponBoxActor> Box;
    int32 RoomIndex = 0;
    Box = TreasureBox[RoomIndex];
    AWeaponBoxActor* SpawnedBox = this->GetWorld()->SpawnActor<AWeaponBoxActor>(Box);
    SpawnedBox->SetActorLocation(TreasureBoxSpawnPoint->GetComponentLocation());
    SpawnedBox->SetActorRotation(TreasureBoxSpawnPoint->GetComponentRotation());
}

void ADungeonRoom7::BeginPlay()
{
    Super::BeginPlay();

    AActor* FoundGenerator = UGameplayStatics::GetActorOfClass(GetWorld(), ADungeonGanarator::StaticClass());
    ADungeonGanarator* Generator = Cast<ADungeonGanarator>(FoundGenerator);

    if (Generator)
    {
        // 제너레이터의 종료 델리게이트에 내 함수 바인딩
        Generator->EndedCreate.AddDynamic(this, &ADungeonRoom7::SpawnTreasureBox);
    }
}

void ADungeonRoom7::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (SpawnedBoxInstance && IsValid(SpawnedBoxInstance))
    {
        SpawnedBoxInstance->Destroy();
    }

    Super::EndPlay(EndPlayReason);
}
