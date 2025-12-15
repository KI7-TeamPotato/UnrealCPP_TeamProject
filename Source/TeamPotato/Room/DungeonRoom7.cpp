// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonRoom7.h"

ADungeonRoom7::ADungeonRoom7()
{
	ExitArrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow1"));
	ExitArrow2 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow2"));
	ClosingWall1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClosingWall1"));
	ClosingWall2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClosingWall2"));

	ExitArrow1->SetupAttachment(ExitPointsFolder);
	ExitArrow2->SetupAttachment(ExitPointsFolder);
	ClosingWall1->SetupAttachment(GeometryFolder);
	ClosingWall2->SetupAttachment(GeometryFolder);
}
