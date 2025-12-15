// Fill out your copyright notice in the Description page of Project Settings.


#include "TestDungeonAssetMap.h"

ATestDungeonAssetMap::ATestDungeonAssetMap()
{
	Cube_9 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CUBE9"));
	Cube_10 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CUBE10"));
	Cube_11 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CUBE11"));
	Cube_12 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CUBE12"));
	Cube_13 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CUBE13"));

	ExitArrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow1"));
	ExitArrow2 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow2"));

	ExitArrow1->SetupAttachment(ExitPointsFolder);
	ExitArrow2->SetupAttachment(ExitPointsFolder);

	Cube_9->SetupAttachment(GeometryFolder);
	Cube_10->SetupAttachment(GeometryFolder);
	Cube_11->SetupAttachment(GeometryFolder);
	Cube_12->SetupAttachment(GeometryFolder);
	Cube_13->SetupAttachment(GeometryFolder);


}
