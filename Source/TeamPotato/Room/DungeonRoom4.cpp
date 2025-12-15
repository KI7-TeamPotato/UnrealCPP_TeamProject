// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonRoom4.h"
#include "Components/ArrowComponent.h"

ADungeonRoom4::ADungeonRoom4()
{
	ExitArrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow1"));
	ExitArrow2 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow2"));
	ExitArrow3 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow3"));
	ClosingWall1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClosingWall1"));
	ClosingWall2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClosingWall2"));

	ExitArrow1->SetupAttachment(ExitPointsFolder);
	ExitArrow2->SetupAttachment(ExitPointsFolder);
	ExitArrow3->SetupAttachment(ExitPointsFolder);
	ClosingWall1->SetupAttachment(GeometryFolder);
	ClosingWall2->SetupAttachment(GeometryFolder);
}
