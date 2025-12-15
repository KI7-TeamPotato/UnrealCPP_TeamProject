// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonRoom3.h"
#include "Components/ArrowComponent.h"

ADungeonRoom3::ADungeonRoom3()
{
	ExitArrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow1"));
	ClosingWall1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClosingWall1"));
	ClosingWall2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClosingWall2"));

	ExitArrow1->SetupAttachment(ExitPointsFolder);
	ClosingWall1->SetupAttachment(GeometryFolder);
	ClosingWall2->SetupAttachment(GeometryFolder);
}
