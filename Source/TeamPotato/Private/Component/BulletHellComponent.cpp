#include "Component/BulletHellComponent.h"
#include "Enemy/Bullet/EnemyProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UBulletHellComponent::UBulletHellComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UBulletHellComponent::SpawnProjectile(FVector Location, FRotator Rotation, float Speed)
{
    if (!ProjectileClass) return;

    UWorld* World = GetWorld();
    if (World)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = GetOwner();

        AEnemyProjectile* Bullet = World->SpawnActor<AEnemyProjectile>(ProjectileClass, Location, Rotation, SpawnParams);
        if (Bullet)
        {
            // 속도 덮어쓰기
            Bullet->ProjectileMovement->Velocity = Rotation.Vector() * Speed;
        }
    }
}

void UBulletHellComponent::SpawnCircleSpiraPattern(int32 NumProjectiles, float Speed, float OffsetAngle)
{
    if (NumProjectiles <= 0) return;

    const float AngleStep = 360.0f / NumProjectiles;
    FVector SpawnLocation = GetOwner()->GetActorLocation();

    for (int32 i = 0; i < NumProjectiles; i++)
    {
        float FinalAngle = (i * AngleStep) + OffsetAngle;

        FRotator SpawnRotation = FRotator(0.0f, FinalAngle, 0.0f);
        SpawnProjectile(SpawnLocation, SpawnRotation, Speed);
    }
}

void UBulletHellComponent::SpawnThreeWayShot(float Speed)
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (!PlayerPawn) return;

    FVector SpawnLocation = GetOwner()->GetActorLocation();
    FVector PlayerLocation = PlayerPawn->GetActorLocation();

    FRotator LookAtRotation = (PlayerLocation - SpawnLocation).Rotation();

    float AngleOffset = -15.0f;

    for (int32 i = 0; i < 3; i++)
    {
        FRotator SpawnRotation = LookAtRotation;
        SpawnRotation.Yaw += AngleOffset;

        SpawnProjectile(SpawnLocation, SpawnRotation, Speed);

        AngleOffset += 15.0f;
    }
}

void UBulletHellComponent::SpawnRainPattern(int32 NumProjectiles, float AreaWidth, float SpawnHeight, float Speed, float ForwardOffset)
{
    if (NumProjectiles <= 1) return;

    FVector OwnerLoc = GetOwner()->GetActorLocation();
    FVector ForwardDir = GetOwner()->GetActorForwardVector();

    FVector CenterLocation = OwnerLoc + (ForwardDir * ForwardOffset);
    CenterLocation.Z += SpawnHeight;

    float StepDistance = AreaWidth / (float)(NumProjectiles - 1);

    FVector RightDir = GetOwner()->GetActorRightVector();

    FVector StartLocation = CenterLocation - (RightDir * (AreaWidth / 2.0f));

    FRotator DownwardRotation = FRotator(-90.0f, 0.0f, 0.0f);

    for (int32 i = 0; i < NumProjectiles; i++)
    {
        FVector SpawnPos = StartLocation + (RightDir * (i * StepDistance));
        SpawnProjectile(SpawnPos, DownwardRotation, Speed);
    }
}

void UBulletHellComponent::SpawnGridAtLocation(TSubclassOf<AActor> ActorToSpawn,
    FVector TargetLocation, int32 Rows, int32 Cols, float Spacing)
{
    if (!ActorToSpawn) return;

    float HalfWidth = ((Rows - 1) * Spacing) / 2.0f;
    float HalfDepth = ((Cols - 1) * Spacing) / 2.0f;

    FVector StartPos = TargetLocation;
    StartPos.X -= HalfWidth;
    StartPos.Y -= HalfDepth;
    for (int32 i = 0; i < Rows; i++)
    {
        for (int32 j = 0; j < Cols; j++)
        {
            FVector SpawnPos = StartPos;
            SpawnPos.X += i * Spacing;
            SpawnPos.Y += j * Spacing;

            FRotator SpawnRot = FRotator(-90.0f, 0.0f, 0.0f);

            GetWorld()->SpawnActor<AActor>(ActorToSpawn, SpawnPos, SpawnRot);
        }
    }
}



void UBulletHellComponent::SpawnCircleSpiraPatternAtLocation(FVector CenterLocation, int32 NumProjectiles, float Speed, float OffsetAngle)
{
    if (NumProjectiles <= 0) return;

    const float AngleStep = 360.0f / NumProjectiles;

    for (int32 i = 0; i < NumProjectiles; i++)
    {
        float FinalAngle = (i * AngleStep) + OffsetAngle;

        FRotator SpawnRotation = FRotator(0.0f, FinalAngle, 0.0f);

        SpawnProjectile(CenterLocation, SpawnRotation, Speed);
    }
}

void UBulletHellComponent::SpawnWaterSplash(TSubclassOf<AActor> ActorToSpawn,
    FVector Origin, int32 Count, float MinSpeed, float MaxSpeed)
{
    if (!ActorToSpawn) return;

    FVector FixedDir = FVector(0.0f, 0.0f, 1.0f); 
    float FixedSpread = 55.0f;                   

    for (int32 i = 0; i < Count; i++)
    {
        FVector RandomDir = FMath::VRandCone(FixedDir, FMath::DegreesToRadians(FixedSpread));

        float RandomSpeed = FMath::RandRange(MinSpeed, MaxSpeed);

        FRotator SpawnRotation = RandomDir.Rotation();

        AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorToSpawn, Origin, SpawnRotation);

        if (SpawnedActor)
        {
            UProjectileMovementComponent* PMC = SpawnedActor->FindComponentByClass<UProjectileMovementComponent>();
            if (PMC)
            {
                // 속도 적용
                PMC->InitialSpeed = RandomSpeed;
                PMC->MaxSpeed = RandomSpeed;
                PMC->Velocity = RandomDir * RandomSpeed;

                if (PMC->ProjectileGravityScale == 0.0f)
                {
                    PMC->ProjectileGravityScale = 0.5f;
                }
            }
        }
    }
}


void UBulletHellComponent::SpawnSpiralShot(UPARAM(ref) float& CurrentAngle, float AngleStep, float Speed)
{
    FVector SpawnLocation = GetOwner()->GetActorLocation();
    FRotator SpawnRotation = FRotator(0.0f, CurrentAngle, 0.0f);

    SpawnProjectile(SpawnLocation, SpawnRotation, Speed);

    // 각도 갱신 (다음 발사를 위해)
    CurrentAngle += AngleStep;
    if (CurrentAngle >= 360.0f) CurrentAngle -= 360.0f;
}


