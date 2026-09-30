#include "Combat/SummonedTurret.h"
#include "Combat/TurretProjectile.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Enemy/EnemyCharacter.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "TimerManager.h"

ASummonedTurret::ASummonedTurret()
{
    PrimaryActorTick.bCanEverTick = false;
    Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
    SetRootComponent(Collision);
    Collision->InitBoxExtent(FVector(35.0f, 35.0f, 50.0f));
    Collision->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    BaseMesh->SetupAttachment(Collision);
    BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    AimPivot = CreateDefaultSubobject<USceneComponent>(TEXT("AimPivot"));
    AimPivot->SetupAttachment(Collision);
    AimPivot->SetRelativeLocation(FVector(0.0f, 0.0f, 40.0f));
    HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
    HeadMesh->SetupAttachment(AimPivot);
    HeadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Muzzle = CreateDefaultSubobject<UArrowComponent>(TEXT("Muzzle"));
    Muzzle->SetupAttachment(AimPivot);
    Muzzle->SetRelativeLocation(FVector(70.0f, 0.0f, 0.0f));
    ProjectileClass = ATurretProjectile::StaticClass();
}

void ASummonedTurret::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    MaxHealth = FMath::IsFinite(MaxHealth) ? FMath::Max(1.0f, MaxHealth) : 100.0f;
    CurrentHealth = MaxHealth;
    DetectionRange = FMath::IsFinite(DetectionRange) ? FMath::Max(1.0f, DetectionRange) : 1200.0f;
    FireInterval = FMath::IsFinite(FireInterval) ? FMath::Max(0.05f, FireInterval) : 1.0f;
    MaxLifetime = FMath::IsFinite(MaxLifetime) ? FMath::Max(0.1f, MaxLifetime) : 60.0f;
}

void ASummonedTurret::BeginPlay()
{
    Super::BeginPlay();
    if (!HasAuthority()) return;
    if (!GetInstigator()) SetInstigator(Cast<APawn>(GetOwner()));
    Summoner = GetInstigator() ? static_cast<AActor*>(GetInstigator()) : GetOwner();
    if (Summoner.IsValid())
    {
        Summoner->OnDestroyed.AddDynamic(this, &ThisClass::OnSummonerDestroyed);
        if (bReplaceExistingTurret)
        {
            TArray<ASummonedTurret*> PreviousTurrets;
            for (TActorIterator<ASummonedTurret> It(GetWorld()); It; ++It)
            {
                if (*It != this && !It->IsActorBeingDestroyed() && It->Summoner == Summoner)
                    PreviousTurrets.Add(*It);
            }
            for (ASummonedTurret* Previous : PreviousTurrets) Previous->Destroy();
        }
    }
    SetLifeSpan(MaxLifetime);
    GetWorldTimerManager().SetTimer(AttackTimer, this, &ThisClass::AttackTimerTick, FireInterval, true);
}

void ASummonedTurret::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(AttackTimer);
    if (Summoner.IsValid()) Summoner->OnDestroyed.RemoveDynamic(this, &ThisClass::OnSummonerDestroyed);
    Super::EndPlay(EndPlayReason);
}

void ASummonedTurret::OnSummonerDestroyed(AActor* DestroyedActor)
{
    if (HasAuthority()) Destroy();
}

float ASummonedTurret::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
    AController* EventInstigator, AActor* DamageCauser)
{
    if (!HasAuthority() || !CanBeDamaged() || IsActorBeingDestroyed() || CurrentHealth <= 0.0f ||
        !FMath::IsFinite(DamageAmount) || DamageAmount <= 0.0f) return 0.0f;
    const float ActualDamage = FMath::Min(CurrentHealth, DamageAmount);
    CurrentHealth -= ActualDamage;
    Super::TakeDamage(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
    OnTurretDamaged(ActualDamage);
    if (CurrentHealth <= 0.0f) Destroy();
    return ActualDamage;
}

bool ASummonedTurret::HasLineOfSight(const AEnemyCharacter* Enemy, const FVector& From) const
{
    FCollisionQueryParams Params(SCENE_QUERY_STAT(TurretSight), false, this);
    if (Summoner.IsValid()) Params.AddIgnoredActor(Summoner.Get());
    FHitResult Hit;
    return !GetWorld()->LineTraceSingleByChannel(Hit, From, Enemy->GetActorLocation(), ECC_Visibility, Params) ||
        Hit.GetActor() == Enemy;
}

AEnemyCharacter* ASummonedTurret::FindNearestEnemy() const
{
    if (!GetWorld() || IsActorBeingDestroyed() || CurrentHealth <= 0.0f) return nullptr;
    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(TurretTarget), false, this);
    GetWorld()->OverlapMultiByObjectType(Overlaps, GetActorLocation(), FQuat::Identity,
        FCollisionObjectQueryParams::AllDynamicObjects, FCollisionShape::MakeSphere(DetectionRange), Params);
    AEnemyCharacter* Closest = nullptr;
    float ClosestDistance = FMath::Square(DetectionRange);
    TSet<AActor*> Visited;
    for (const FOverlapResult& Overlap : Overlaps)
    {
        AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Overlap.GetActor());
        if (!IsValid(Enemy) || Visited.Contains(Enemy)) continue;
        Visited.Add(Enemy);
        if (Enemy->IsActorBeingDestroyed() || Enemy->IsHidden() || !Enemy->CanBeDamaged() || Enemy->GetCombatHealth() <= 0.0f) continue;
        const float Distance = FVector::DistSquared(GetActorLocation(), Enemy->GetActorLocation());
        if (Distance <= ClosestDistance && HasLineOfSight(Enemy, AimPivot->GetComponentLocation()))
        {
            Closest = Enemy;
            ClosestDistance = Distance;
        }
    }
    return Closest;
}

void ASummonedTurret::AttackTimerTick()
{
    FireAtNearestEnemy();
}

ATurretProjectile* ASummonedTurret::FireAtNearestEnemy()
{
    if (!HasAuthority() || !ProjectileClass || IsActorBeingDestroyed() || CurrentHealth <= 0.0f ||
        GetWorld()->GetTimeSeconds() + UE_SMALL_NUMBER < NextFireTime) return nullptr;
    AEnemyCharacter* Enemy = FindNearestEnemy();
    if (!Enemy) return nullptr;
    AimPivot->SetWorldRotation((Enemy->GetActorLocation() - AimPivot->GetComponentLocation()).Rotation());
    const FVector Origin = Muzzle->GetComponentLocation();
    if (!HasLineOfSight(Enemy, Origin)) return nullptr;
    // An offset muzzle must not shoot through a wall between the pivot and barrel tip.
    FCollisionQueryParams Params(SCENE_QUERY_STAT(TurretBarrel), false, this);
    if (Summoner.IsValid()) Params.AddIgnoredActor(Summoner.Get());
    if (GetWorld()->LineTraceTestByChannel(AimPivot->GetComponentLocation(), Origin, ECC_Visibility, Params)) return nullptr;
    const FVector Direction = (Enemy->GetActorLocation() - Origin).GetSafeNormal();
    if (Direction.IsNearlyZero()) return nullptr;
    FActorSpawnParameters Spawn;
    Spawn.Owner = this;
    Spawn.Instigator = GetInstigator();
    Spawn.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    ATurretProjectile* Projectile = GetWorld()->SpawnActor<ATurretProjectile>(ProjectileClass, Origin, Direction.Rotation(), Spawn);
    if (!IsValid(Projectile)) return nullptr;
    if (!Projectile->Launch(Direction))
    {
        Projectile->Destroy();
        return nullptr;
    }
    NextFireTime = GetWorld()->GetTimeSeconds() + FireInterval;
    OnProjectileFired(Projectile);
    return Projectile;
}
