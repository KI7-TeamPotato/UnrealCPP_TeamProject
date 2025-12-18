// Door.cpp

#include "Door.h"
#include "Components/BoxComponent.h"
#include "TeamPotato/Room/RoomBase.h"

ADoor::ADoor()
{
    PrimaryActorTick.bCanEverTick = true;

    DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
    RootComponent = DefaultSceneRoot;

    Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));

    Door->SetupAttachment(DefaultSceneRoot);
}

void ADoor::BeginPlay()
{
    Super::BeginPlay();
    StartLocation = Door->GetRelativeLocation();

    MoveDoor(true);
}

void ADoor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bCanMove)
    {
        FVector CurrentLocation = Door->GetRelativeLocation();
        FVector NewLoc = FMath::VInterpTo(CurrentLocation, EndLocation, DeltaTime, OpenSpeed);
        Door->SetRelativeLocation(NewLoc);

        if (FVector::Dist(CurrentLocation, EndLocation) < 0.1f)
        {
            bCanMove = false;
            Door->SetRelativeLocation(EndLocation);
        }
    }
}

// 내부 이동 함수
void ADoor::MoveDoor(bool bOpen)
{
    if (bOpen)
    {
        EndLocation = StartLocation - FVector(0.0f, 0.0f, 500.0f);
    }
    else
    {
        EndLocation = StartLocation;
    }
    bCanMove = true;
}

void ADoor::OpenDoor()
{
    MoveDoor(true);
}

void ADoor::CloseDoor()
{
    MoveDoor(false);
}

void ADoor::SetOwningRoom(ARoomBase* NewRoom)
{
    OwningRoom = NewRoom;
}