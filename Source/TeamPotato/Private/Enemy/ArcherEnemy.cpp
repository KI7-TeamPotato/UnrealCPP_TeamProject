// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/ArcherEnemy.h"
#include "Enemy/BulletBase.h"
#include "Kismet/GameplayStatics.h"

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
    // 1. 타겟(플레이어) 확인
    ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(this, 0);
    if (!PlayerChar) return;

    // 2. 풀에서 사용 가능한(비활성) 총알 찾기
    ABulletBase* BulletToFire = nullptr;
    for (ABulletBase* Bullet : BulletPool)
    {
        if (Bullet && !Bullet->IsActive())
        {
            BulletToFire = Bullet;
            break;
        }
    }

    if (BulletToFire)
    {
        BulletToFire->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

        FVector StartLocation = GetActorLocation() + (GetActorForwardVector() * 100.0f);
        FVector Direction = (PlayerChar->GetActorLocation() - StartLocation).GetSafeNormal();

        BulletToFire->Launch(StartLocation, Direction);
    }
}

