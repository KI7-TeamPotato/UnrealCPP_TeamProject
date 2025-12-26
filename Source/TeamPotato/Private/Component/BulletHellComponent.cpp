#include "Component/BulletHellComponent.h"
#include "Enemy/Bullet/EnemyProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
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


void UBulletHellComponent::SpawnSpiralShot(UPARAM(ref) float& CurrentAngle, float AngleStep, float Speed)
{
    FVector SpawnLocation = GetOwner()->GetActorLocation();
    FRotator SpawnRotation = FRotator(0.0f, CurrentAngle, 0.0f);

    SpawnProjectile(SpawnLocation, SpawnRotation, Speed);

    // 각도 갱신 (다음 발사를 위해)
    CurrentAngle += AngleStep;
    if (CurrentAngle >= 360.0f) CurrentAngle -= 360.0f;
}


