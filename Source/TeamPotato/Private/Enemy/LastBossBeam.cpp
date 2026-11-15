#include "Enemy/LastBoss.h"
#include "Components/SkeletalMeshComponent.h"

namespace
{
    const FName BeamSocket(TEXT("Beam"));
}

void ALastBoss::BeginBeamSweep(AActor* Beam)
{
    EndBeamSweep();
    if (!IsValid(Beam)) return;

    SweepingBeam = Beam;
    SweepStartBodyRotation = GetActorQuat();
    SweepStartBeamRotation = GetMesh()->GetSocketQuaternion(BeamSocket);
    UpdateBeamSweep(0.0f);
}

void ALastBoss::UpdateBeamSweep(float PlaybackSeconds)
{
    if (!SweepStartBodyRotation.IsSet()) return;

    const FQuat BodyOffset(FVector::UpVector, FMath::DegreesToRadians(BodyYawDegreesPerSecond * PlaybackSeconds));
    SetActorRotation(SweepStartBodyRotation.GetValue() * BodyOffset);

    if (AActor* Beam = SweepingBeam.Get())
    {
        const FQuat BeamOffset(FVector::UpVector, FMath::DegreesToRadians(BeamYawDegreesPerSecond * PlaybackSeconds));
        Beam->SetActorLocationAndRotation(GetMesh()->GetSocketLocation(BeamSocket), SweepStartBeamRotation * BeamOffset);
    }
}

void ALastBoss::EndBeamSweep()
{
    if (!SweepStartBodyRotation.IsSet()) return;

    SetActorRotation(SweepStartBodyRotation.GetValue());
    SweepStartBodyRotation.Reset();
    if (AActor* Beam = SweepingBeam.Get()) Beam->Destroy();
    SweepingBeam.Reset();
}
