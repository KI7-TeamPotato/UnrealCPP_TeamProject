// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/ArcherEnemy.h"
#include "Enemy/BulletBase.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Combat/CombatFunctionLibrary.h"
#include "Combat/CombatTags.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "Combat/CombatAbilitySystemComponent.h"
#include "Components/WidgetComponent.h"

AArcherEnemy::AArcherEnemy()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AArcherEnemy::BeginPlay()
{
    Super::BeginPlay();

    InitBulletPool();
}

void AArcherEnemy::InitBulletPool()
{
    if (!BulletClass) return;

    for (int32 i = 0; i < PoolSize; i++) 
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = this;

        // 생성 후 숨겨둠 (BulletBase 내부 로직에 의해 자동 Deactivate)
        ABulletBase* NewBullet = GetWorld()->SpawnActor<ABulletBase>(BulletClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

        if (NewBullet)
        {
            NewBullet->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
            BulletPool.Add(NewBullet);
        }
    }
}

void AArcherEnemy::Shoot()
{
    AActor* Target = ResolveRangedTarget();
    if (!HasAuthority() || !UCombatFunctionLibrary::CanActorAttack(this) || !IsLivingRangedTarget(Target)) return;
    // A queued montage notify must not turn the enemy back toward the player after repositioning starts.
    const AAIController* AI = Cast<AAIController>(GetController());
    if (AI && AI->GetMoveStatus() == EPathFollowingStatus::Moving) return;
    if (!HasClearRangedShot(Target)) return;
    const float Distance = FVector::Dist2D(GetActorLocation(), Target->GetActorLocation());
    if (Distance < FMath::Max(0.0f, RetreatStartDistance) || Distance > MaxRangedAttackDistance) return;
    const FRotator Aim = (Target->GetActorLocation() - GetActorLocation()).Rotation();
    SetActorRotation(FRotator(0.0f, Aim.Yaw, 0.0f));

    ABulletBase* BulletToFire = nullptr;
    for (ABulletBase* Bullet : BulletPool)
    {
        if (IsValid(Bullet) && !Bullet->IsActive())
        {
            BulletToFire = Bullet;
            break;
        }
    }

    if (BulletToFire)
    {
        BulletToFire->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

        FVector StartLocation = GetActorLocation() + (GetActorForwardVector() * 100.0f);
        FVector Direction = (Target->GetActorLocation() - StartLocation).GetSafeNormal();

        BulletToFire->Launch(StartLocation, Direction, this->BulletSpeed);
    }
}

bool AArcherEnemy::IsLivingRangedTarget(const AActor* Target)
{
    if (!IsValid(Target) || Target->IsActorBeingDestroyed() || !Target->CanBeDamaged()) return false;
    const auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target);
    if (ASC && ASC->HasMatchingGameplayTag(CombatTags::State_Dead)) return false;
    const auto* Combat = Cast<UCombatAbilitySystemComponent>(ASC);
    return !Combat || Combat->GetHealth() > 0.0f;
}

AActor* AArcherEnemy::ResolveRangedTarget() const
{
    if (const auto* AI = Cast<AAIController>(GetController()))
    {
        if (const auto* BB = AI->GetBlackboardComponent())
        {
            AActor* Target = Cast<AActor>(BB->GetValueAsObject(TEXT("AttackTarget")));
            if (Target != this && IsLivingRangedTarget(Target)) return Target;
        }
    }
    AActor* Player = UGameplayStatics::GetPlayerPawn(this, 0);
    return Player != this && IsLivingRangedTarget(Player) ? Player : nullptr;
}

bool AArcherEnemy::HasClearRangedShot(AActor* Target) const
{
    return HasClearRangedShotFrom(GetActorLocation(), Target);
}

bool AArcherEnemy::HasClearRangedShotFrom(const FVector& Origin, AActor* Target) const
{
    if (!IsLivingRangedTarget(Target) || Target == this || !GetWorld()) return false;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(EnemyRangedShot), false, this);
    TArray<AActor*> Attached;
    GetAttachedActors(Attached, true, true);
    Params.AddIgnoredActors(Attached);
    auto ClearSegment = [&](const FVector& Start, const FVector& End)
    {
        // World-space health bars must not occlude the firing lane.
        for (int32 Attempt = 0; Attempt < 16; ++Attempt)
        {
            FHitResult Hit;
            if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params)) return true;
            if (Cast<UWidgetComponent>(Hit.GetComponent())) Params.AddIgnoredComponent(Hit.GetComponent());
            else return Hit.GetActor() == Target;
        }
        return false;
    };
    const FVector AimForward = (Target->GetActorLocation() - Origin).GetSafeNormal2D();
    const FVector Muzzle = Origin + AimForward * 100.0f;
    return ClearSegment(Origin, Muzzle) && ClearSegment(Muzzle, Target->GetActorLocation());
}

void AArcherEnemy::DefaultAttack()
{
    TryStartRangedAttack(ResolveRangedTarget());
}

bool AArcherEnemy::TryStartRangedAttack(AActor* Target)
{
    if (!HasAuthority() || !AttackMontage || !GetWorld() || !UCombatFunctionLibrary::CanActorAttack(this) ||
        !IsLivingRangedTarget(Target) || GetWorld()->GetTimeSeconds() < NextRangedAttackTime) return false;
    UAnimInstance* Anim = GetMesh()->GetAnimInstance();
    if (!Anim || Anim->Montage_IsPlaying(AttackMontage)) return false;
    const float Distance = FVector::Dist2D(GetActorLocation(), Target->GetActorLocation());
    if (!FMath::IsFinite(Distance) || Distance < FMath::Max(0.0f, RetreatStartDistance) || Distance > MaxRangedAttackDistance) return false;
    const FRotator Facing = (Target->GetActorLocation() - GetActorLocation()).Rotation();
    SetActorRotation(FRotator(0.0f, Facing.Yaw, 0.0f));
    if (GetController()) GetController()->SetControlRotation(GetActorRotation());
    if (!HasClearRangedShot(Target)) return false;
    const float Duration = PlayAnimMontage(AttackMontage);
    if (Duration <= 0.0f) return false;
    const float Interval = FMath::IsFinite(RangedAttackInterval) ? FMath::Max(0.1f, RangedAttackInterval) : 3.0f;
    NextRangedAttackTime = GetWorld()->GetTimeSeconds() + FMath::Max(Interval, Duration + 0.05f);
    return true;
}

