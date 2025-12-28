// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonRoom9.h"
#include "Enemy/BossScarecrow.h"
#include "Kismet/GameplayStatics.h"

ADungeonRoom9::ADungeonRoom9()
{
	ClosingWall1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClosingWall1"));
    PortalPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("PortalPoint"));

	ClosingWall1->SetupAttachment(GeometryFolder);
    PortalPoint->SetupAttachment(OtherArchitecture);

}
void ADungeonRoom9::BeginPlay()
{
    Super::BeginPlay();

    AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ABossScarecrow::StaticClass());

    ABossScarecrow* BossActor = Cast<ABossScarecrow>(FoundActor);

    if (BossActor)
    {
        BossActor->OnScarecrowDead.AddDynamic(this, &ADungeonRoom9::SpawnPortal);
    }
}
void ADungeonRoom9::SpawnPortal()
{
    if (PortalClass && PortalPoint)
    {
        FVector SpawnLocation = PortalPoint->GetComponentLocation();
        FRotator SpawnRotation = PortalPoint->GetComponentRotation();

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;

        GetWorld()->SpawnActor<AActor>(PortalClass, SpawnLocation, SpawnRotation, SpawnParams);

        UE_LOG(LogTemp, Log, TEXT("SpawnPortal"));
    }
}
