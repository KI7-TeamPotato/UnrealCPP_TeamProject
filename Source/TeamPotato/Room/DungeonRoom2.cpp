// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonRoom2.h"
#include "Components/ArrowComponent.h"

ADungeonRoom2::ADungeonRoom2()
{
	ExitArrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow1"));
	ExitArrow2 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow2"));
	ClosingWall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClosingWall"));

	ExitArrow1->SetupAttachment(ExitPointsFolder);
	ExitArrow2->SetupAttachment(ExitPointsFolder);
	ClosingWall->SetupAttachment(GeometryFolder);
}
