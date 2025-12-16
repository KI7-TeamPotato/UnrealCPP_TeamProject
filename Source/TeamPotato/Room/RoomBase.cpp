// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomBase.h"


// Sets default values
ARoomBase::ARoomBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));

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

	BoxCollision->SetupAttachment(OverlapFolder);

	Arrow->bHiddenInGame = false;


}

// Called when the game starts or when spawned
void ARoomBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARoomBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

