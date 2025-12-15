// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonRoom9.h"

ADungeonRoom9::ADungeonRoom9()
{
	ClosingWall1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClosingWall1"));

	ClosingWall1->SetupAttachment(GeometryFolder);
}
