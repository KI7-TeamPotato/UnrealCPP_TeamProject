// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonRoom6.h"
#include "Components/ArrowComponent.h"

ADungeonRoom6::ADungeonRoom6()
{
	ExitArrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow1"));
	ClosingWall1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClosingWall1"));
	ClosingWall2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClosingWall2"));
	BoxCollision2 = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision2"));

	ExitArrow1->SetupAttachment(ExitPointsFolder);
	ClosingWall1->SetupAttachment(GeometryFolder);
	ClosingWall2->SetupAttachment(GeometryFolder);
	BoxCollision2->SetupAttachment(OverlapFolder);


}
