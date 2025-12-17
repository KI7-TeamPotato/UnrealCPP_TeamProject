// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonRoom1.h"
#include "Components/ArrowComponent.h"



ADungeonRoom1::ADungeonRoom1()
{
	ExitArrow1 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow1"));
	ExitArrow2 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow2"));
	ExitArrow3 = CreateDefaultSubobject<UArrowComponent>(TEXT("ExitArrow3"));

	ExitArrow1->SetupAttachment(ExitPointsFolder);
	ExitArrow2->SetupAttachment(ExitPointsFolder);
	ExitArrow3->SetupAttachment(ExitPointsFolder);
}

void ADungeonRoom1::BeginPlay()
{
	Super::BeginPlay();
}
