// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonRoom2_1.h"

ADungeonRoom2_1::ADungeonRoom2_1()
{
	Wall10 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall10"));
	Wall11 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Wall11"));

	Floor5 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor5"));
	Floor6 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor6"));
	Floor7 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor7"));
	Floor8 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor8"));
	Floor9 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Floor9"));

	Piller5 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Piller5"));
	Piller6 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Piller6"));
	Piller7 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Piller7"));
	Piller8 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Piller8"));

	Ceiliing5 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ceiliing5"));
	Ceiliing6 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ceiliing6"));
	Ceiliing7 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ceiliing7"));
	Ceiliing8 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ceiliing8"));
	Ceiliing9 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ceiliing9"));

	Arch1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arch1"));
	Arch2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arch2"));
	Arch3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arch3"));
	Arch4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arch4"));
	Arch5 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arch5"));
	Arch6 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arch6"));
	Arch7 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arch7"));
	Arch8 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arch8"));
	Arch9 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arch9"));
	Arch10 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arch10"));
	Arch11 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arch11"));
	Arch12 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arch12"));

	ExitArrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow1"));

	Wall10->SetupAttachment(WallFolder);
	Wall11->SetupAttachment(WallFolder);

	Floor5->SetupAttachment(FloorFolder);
	Floor6->SetupAttachment(FloorFolder);
	Floor7->SetupAttachment(FloorFolder);
	Floor8->SetupAttachment(FloorFolder);
	Floor9->SetupAttachment(FloorFolder);

	Piller5->SetupAttachment(OtherArchitecture);
	Piller6->SetupAttachment(OtherArchitecture);
	Piller7->SetupAttachment(OtherArchitecture);
	Piller8->SetupAttachment(OtherArchitecture);

	Arch1->SetupAttachment(OtherArchitecture);
	Arch2->SetupAttachment(OtherArchitecture);
	Arch3->SetupAttachment(OtherArchitecture);
	Arch4->SetupAttachment(OtherArchitecture);
	Arch5->SetupAttachment(OtherArchitecture);
	Arch6->SetupAttachment(OtherArchitecture);
	Arch7->SetupAttachment(OtherArchitecture);
	Arch8->SetupAttachment(OtherArchitecture);
	Arch9->SetupAttachment(OtherArchitecture);
	Arch10->SetupAttachment(OtherArchitecture);
	Arch11->SetupAttachment(OtherArchitecture);
	Arch12->SetupAttachment(OtherArchitecture);

	Ceiliing5->SetupAttachment(CeilingFolder);
	Ceiliing6->SetupAttachment(CeilingFolder);
	Ceiliing7->SetupAttachment(CeilingFolder);
	Ceiliing8->SetupAttachment(CeilingFolder);
	Ceiliing9->SetupAttachment(CeilingFolder);

	ExitArrow1->SetupAttachment(ExitPointsFolder);

}
