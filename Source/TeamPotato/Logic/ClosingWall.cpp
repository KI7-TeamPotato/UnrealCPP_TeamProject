// Fill out your copyright notice in the Description page of Project Settings.


#include "ClosingWall.h"

// Sets default values
AClosingWall::AClosingWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ClosingWall = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClosingWall"));
	SetRootComponent(ClosingWall);
	//ClosingWall->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AClosingWall::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AClosingWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

