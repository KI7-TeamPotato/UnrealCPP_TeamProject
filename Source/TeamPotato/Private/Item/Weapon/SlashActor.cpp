// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/SlashActor.h"
#include "Player/TestCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ASlashActor::ASlashActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    SlashCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collsion"));
    SlashCollision->SetCollisionProfileName(TEXT("Projectile"));
    SlashCollision->SetNotifyRigidBodyCollision(true);
    SlashCollision->SetupAttachment(Root);
    SlashCollision->OnComponentHit.AddDynamic(this, &ASlashActor::OnHit);

    SlashEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Effect"));
    SlashEffect->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SlashEffect->SetupAttachment(SlashCollision);

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovementComponent->SetUpdatedComponent(SlashCollision);
    ProjectileMovementComponent->InitialSpeed = 1500.f;
    ProjectileMovementComponent->MaxSpeed = 1500.f;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->bShouldBounce = false;
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
}

// Called when the game starts or when spawned
void ASlashActor::BeginPlay()
{
	Super::BeginPlay();
	
    APawn* PlayerInstigator = GetInstigator();

    if (PlayerInstigator)
    {
        // 1. 이동 중 충돌 무시
        SlashCollision->IgnoreActorWhenMoving(PlayerInstigator, true);

        // 2. 플레이어의 모든 컴포넌트(캡슐, 메쉬 등)와 충돌하지 않도록 설정
        PlayerInstigator->MoveIgnoreActorAdd(this);
    }
}

void ASlashActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    float finalDamage = SlashDamage;
    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    AController* instigator = Cast<APlayerController>(OwnerCharacter->GetController());

    UGameplayStatics::ApplyDamage(OtherActor, finalDamage, instigator, this, DamageType);

    // Destroy the projectile
    Destroy();
}
