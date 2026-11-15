// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonRoom9.h"

ADungeonRoom9::ADungeonRoom9()
{
	ClosingWall1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClosingWall1"));
    PortalPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("PortalPoint"));

	ClosingWall1->SetupAttachment(GeometryFolder);
    PortalPoint->SetupAttachment(OtherArchitecture);

}
void ADungeonRoom9::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (SpawnedPortal.IsValid()) SpawnedPortal->Destroy();
    Super::EndPlay(EndPlayReason);
}
void ADungeonRoom9::SpawnPortal()
{
    if (PortalClass && PortalPoint && !SpawnedPortal.IsValid())
    {
        FVector SpawnLocation = PortalPoint->GetComponentLocation();
        FRotator SpawnRotation = PortalPoint->GetComponentRotation();

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;

        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        SpawnedPortal = GetWorld()->SpawnActor<AActor>(PortalClass, SpawnLocation, SpawnRotation, SpawnParams);

        UE_LOG(LogTemp, Log, TEXT("SpawnPortal"));
    }
}
