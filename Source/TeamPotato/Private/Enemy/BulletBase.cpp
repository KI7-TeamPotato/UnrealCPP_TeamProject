#include "TeamPotato/Public/Enemy/BulletBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"

ABulletBase::ABulletBase()
{
    PrimaryActorTick.bCanEverTick = false; // 틱은 필요 없으면 끕니다 (최적화)
    bIsActive = false;

    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
    SetRootComponent(CapsuleComp);
    CapsuleComp->InitCapsuleSize(10.0f, 20.0f);
    CapsuleComp->SetCollisionProfileName(TEXT("NoCollision"));

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    MeshComp->SetupAttachment(CapsuleComp);
    MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
    MovementComp->UpdatedComponent = CapsuleComp;
    MovementComp->InitialSpeed = 3000.0f;
    MovementComp->MaxSpeed = 3000.0f;
    MovementComp->bRotationFollowsVelocity = true;
    MovementComp->ProjectileGravityScale = 0.0f; 
    MovementComp->bAutoActivate = false; 
}

void ABulletBase::BeginPlay()
{
    Super::BeginPlay();

    Deactivate();
}

void ABulletBase::Launch(const FVector& StartLocation, const FVector& Direction)
{
    if (bIsActive) return; // 이미 날아가고 있다면 무시 (혹은 강제 초기화 선택)

    bIsActive = true;

    SetActorLocation(StartLocation);
    SetActorRotation(Direction.Rotation());

    SetActorHiddenInGame(false);
    CapsuleComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

    MovementComp->Activate();
    MovementComp->Velocity = Direction.GetSafeNormal() * MovementComp->InitialSpeed;

    GetWorld()->GetTimerManager().SetTimer(LifeTimerHandle, this, &ABulletBase::OnLifeTimeExpired, MaxLifeTime, false);
}

void ABulletBase::Deactivate()
{
    bIsActive = false;

    GetWorld()->GetTimerManager().ClearTimer(LifeTimerHandle);

    SetActorHiddenInGame(true);

    CapsuleComp->SetCollisionProfileName(TEXT("NoCollision"));

    MovementComp->StopMovementImmediately();
    MovementComp->Deactivate();
}

void ABulletBase::OnLifeTimeExpired()
{
    Deactivate();
}