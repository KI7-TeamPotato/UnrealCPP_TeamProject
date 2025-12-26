// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomBase.h"
#include "GameFramework/Character.h"
#include "Player/TestCharacter.h"

// Sets default values
ARoomBase::ARoomBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	DefaultScenRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultScenRoot"));
	GeometryFolder = CreateDefaultSubobject<USceneComponent>(TEXT("GeometryFolder"));
	OverlapFolder = CreateDefaultSubobject<USceneComponent>(TEXT("OverlapFolder"));
	ExitPointsFolder = CreateDefaultSubobject<USceneComponent>(TEXT("ExitPointsFolder"));
	SpawnPointsFolder = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPointsFolder"));
	WallFolder = CreateDefaultSubobject<USceneComponent>(TEXT("WallFolder"));
	CeilingFolder = CreateDefaultSubobject<USceneComponent>(TEXT("CeilingFolder"));
	FloorFolder = CreateDefaultSubobject<USceneComponent>(TEXT("FloorFolder"));
	OtherArchitecture = CreateDefaultSubobject<USceneComponent>(TEXT("OtherArchitecture"));

	Wall1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall1"));
	Wall2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall2"));
	Wall3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall3"));
	Wall4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall4"));
	Wall5 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall5"));
	Wall6 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall6"));
	Wall7 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall7"));
	Wall8 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall8"));
	Floor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor"));
	Floor2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor2"));
	Floor3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor3"));
	Floor4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor4"));
	Ceiling1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ceiling1"));
	Ceiling2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ceiling2"));
	Ceiling3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ceiling3"));
	Ceiling4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ceiling4"));
	DoorWay1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorWay1"));
	DoorWay2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorWay2"));
	DoorWay3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorWay3"));
	Piller1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Piller1"));
	Piller2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Piller2"));
	Piller3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Piller3"));
	Piller4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Piller4"));
    WaveSystemComp = CreateDefaultSubobject<UWaveComponent>(TEXT("WaveSystemComp"));
    SpawnPoint1 = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPoint1"));
    SpawnPoint2 = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPoint2"));
    SpawnPoint3 = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPoint3"));
    SpawnPoint4 = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPoint4"));
    SpawnPoint5 = CreateDefaultSubobject<UArrowComponent>(TEXT("SpawnPoint5"));
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
    OnEnterRoomCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("OnEnterRoomCollision"));

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	SetRootComponent(DefaultScenRoot);

	//DefaultScenRoot->SetupAttachment(RootComponent);
	GeometryFolder->SetupAttachment(RootComponent);
	OverlapFolder->SetupAttachment(RootComponent);
	ExitPointsFolder->SetupAttachment(RootComponent);
	SpawnPointsFolder->SetupAttachment(RootComponent);
	WallFolder->SetupAttachment(GeometryFolder);
	CeilingFolder->SetupAttachment(GeometryFolder);
	FloorFolder->SetupAttachment(GeometryFolder);
	OtherArchitecture->SetupAttachment(GeometryFolder);

	Arrow->SetupAttachment(RootComponent);
	Wall1->SetupAttachment(WallFolder);
	Wall2->SetupAttachment(WallFolder);
	Wall3->SetupAttachment(WallFolder);
	Wall4->SetupAttachment(WallFolder);
	Wall5->SetupAttachment(WallFolder);
	Wall6->SetupAttachment(WallFolder);
	Wall7->SetupAttachment(WallFolder);
	Wall8->SetupAttachment(WallFolder);
	Floor->SetupAttachment(FloorFolder);
	Floor2->SetupAttachment(FloorFolder);
	Floor3->SetupAttachment(FloorFolder);
	Floor4->SetupAttachment(FloorFolder);
	Ceiling1->SetupAttachment(CeilingFolder);
	Ceiling2->SetupAttachment(CeilingFolder);
	Ceiling3->SetupAttachment(CeilingFolder);
	Ceiling4->SetupAttachment(CeilingFolder);
	DoorWay1->SetupAttachment(WallFolder);
	DoorWay2->SetupAttachment(WallFolder);
	DoorWay3->SetupAttachment(WallFolder);
	Piller1->SetupAttachment(OtherArchitecture);
	Piller2->SetupAttachment(OtherArchitecture);
	Piller3->SetupAttachment(OtherArchitecture);
	Piller4->SetupAttachment(OtherArchitecture);
    SpawnPoint1->SetupAttachment(SpawnPointsFolder);
    SpawnPoint2->SetupAttachment(SpawnPointsFolder);
    SpawnPoint3->SetupAttachment(SpawnPointsFolder);
    SpawnPoint4->SetupAttachment(SpawnPointsFolder);
    SpawnPoint5->SetupAttachment(SpawnPointsFolder);

	BoxCollision->SetupAttachment(OverlapFolder);
    OnEnterRoomCollision->SetupAttachment(SpawnPointsFolder);
	Arrow->bHiddenInGame = false;

    OnEnterRoomCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ARoomBase::BeginPlay()
{
    Super::BeginPlay();

    if (WaveSystemComp)
    {
        WaveSystemComp->OnAllWavesCleared.AddDynamic(this, &ARoomBase::HandleBattleEnd);
    }

    if (OnEnterRoomCollision)
    {
        OnEnterRoomCollision->OnComponentBeginOverlap.AddDynamic(this, &ARoomBase::OnRoomTriggerBeginOverlap);
    }
}

void ARoomBase::RegisterDoor(ADoor* NewDoor)
{
    if (NewDoor)
    {
        RoomDoors.AddUnique(NewDoor);
        // 문에게도 "너의 주인은 나야"라고 알려줌 (아래 Door 수정사항 필요)
        NewDoor->SetOwningRoom(this);
    }
}

void ARoomBase::SetGlobalDoorState(bool bShouldClose)
{
    // 월드의 모든 ARoomBase 액터를 찾습니다.
    TArray<AActor*> AllRooms;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomBase::StaticClass(), AllRooms);

    for (AActor* RoomActor : AllRooms)
    {
        ARoomBase* Room = Cast<ARoomBase>(RoomActor);
        if (Room)
        {
            if (bShouldClose)
            {
                // 모든 방 닫기 (클리어 여부 상관없이 전투 중이면 봉쇄)
                Room->CloseAllRoomDoors();
            }
            else
            {
                // 모든 방 열기
                Room->OpenAllRoomDoors();
            }
        }
    }
}

void ARoomBase::OpenAllRoomDoors()
{
    for (ADoor* Door : RoomDoors)
    {
        if (Door)
        {
            // 문 자체의 이동 로직만 호출 (재귀 호출 방지를 위해 OpenDoor 직접 호출)
            Door->OpenDoor();
        }
    }
}

void ARoomBase::CloseAllRoomDoors()
{
    bIsRoomClear = false;

    for (ADoor* Door : RoomDoors)
    {
        if (Door)
        {
            // 문 자체의 이동 로직만 호출 (재귀 호출 방지를 위해 OpenDoor 직접 호출)
            Door->CloseDoor();
        }
    }
}

void ARoomBase::ActivateBattleTrigger()
{
    if (OnEnterRoomCollision)
    {
        OnEnterRoomCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

        OnEnterRoomCollision->UpdateOverlaps();
    }
}

void ARoomBase::HandleBattleEnd()
{
    UE_LOG(LogTemp, Log, TEXT("All Waves Cleared"));
    SetGlobalDoorState(false);
    bIsStartBattle = false;
    bIsRoomClear = true;
}


void ARoomBase::OnRoomTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor, 
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, 
    bool bFromSweep, 
    const FHitResult& SweepResult)
{
    if (bIsStartBattle || bIsRoomClear)
    {
        return;
    }
    //나중에 실제 플레이어 캐릭터 클래스로 바꿔야 함
    ACharacter* PlayerCharacter = Cast<ATestCharacter>(OtherActor);
    if (PlayerCharacter && WaveSystemComp)
    {
        bIsStartBattle = true;

        UE_LOG(LogTemp, Warning, TEXT("Battle Start"));

        SetGlobalDoorState(true);

        TArray<UArrowComponent*> Points;
        Points.Add(SpawnPoint1);
        Points.Add(SpawnPoint2);
        Points.Add(SpawnPoint3);
        Points.Add(SpawnPoint4);
        Points.Add(SpawnPoint5);

        WaveSystemComp->StartWaveSystem(Points);

        OnEnterRoomCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}
