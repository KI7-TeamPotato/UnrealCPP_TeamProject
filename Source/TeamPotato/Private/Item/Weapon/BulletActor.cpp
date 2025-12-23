// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/BulletActor.h"
#include "Player/TestCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
ABulletActor::ABulletActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 충돌 컴포넌트 초기화
	BulletCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	BulletCollision->InitSphereRadius(15.0f);
	BulletCollision->SetCollisionProfileName(TEXT("Projectile"));
	BulletCollision->SetNotifyRigidBodyCollision(true); // Enable hit events
	RootComponent = BulletCollision;

	// 발사체 컴포넌트 추기화
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(BulletCollision);
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.3f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	// 메쉬 초기화
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	BulletMesh->SetupAttachment(RootComponent);
	BulletMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 트레일 초기화
    TrailEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Trail"));
    TrailEffect->SetupAttachment(BulletMesh);
    TrailEffect->Activate();

	// 충돌 델리게이트 설정
	BulletCollision->OnComponentHit.AddDynamic(this, &ABulletActor::OnHit);
	InitialLifeSpan = 5.0f;
}

void ABulletActor::GunFire(const FVector& ShootDirection, float Speed, float Damage)
{
	ProjectileMovementComponent->InitialSpeed = Speed;
	ProjectileMovementComponent->MaxSpeed = Speed;
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
	BulletDamage = Damage;
}

// Called when the game starts or when spawned
void ABulletActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (AActor* OwnerActor = GetOwner())
	{
		BulletCollision->IgnoreActorWhenMoving(OwnerActor, true);
	}
}

void ABulletActor::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Display, TEXT("On hit bullet"));

	if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
	{
		OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 100.0f, Hit.ImpactPoint);
	}

	// Spawn impact effect
	if (HitEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			HitEffect,
			Hit.ImpactPoint,
			Hit.ImpactNormal.Rotation()
		);
	}

	float finalDamage = BulletDamage;
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	AController* instigator = Cast<APlayerController>(OwnerCharacter->GetController());

	UGameplayStatics::ApplyDamage(OtherActor, finalDamage, instigator, this, DamageType);

	// Destroy the projectile
	Destroy();
}

