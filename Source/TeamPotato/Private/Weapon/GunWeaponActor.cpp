// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/GunWeaponActor.h"
#include "Weapon/BulletActor.h"
#include "Weapon/WeaponComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

// Sets default values
AGunWeaponActor::AGunWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	RootComponent = GunMesh;
}

void AGunWeaponActor::Fire()
{
	// 오류 검사
	if (!OwnerWeaponComponent || !BulletClass) return;

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) return;

	USkeletalMeshComponent* OwnerMesh = OwnerCharacter->GetMesh();
	if (!OwnerMesh) return;

	APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!PC) return;

	// 총구 위치 계산
	FTransform SocketTransform = GunMesh->GetSocketTransform("FirePoint", RTS_World);
	FVector MuzzleLocation = SocketTransform.GetLocation();

	// 화면 중심 좌표 계산
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	FVector2D ScreenCenter(ViewportSize * 0.5f);

	// 화면 중심에서 월드 방향 가져오기
	FVector WorldLocation, WorldDirection;
	UGameplayStatics::DeprojectScreenToWorld(
		PC,
		ScreenCenter,
		WorldLocation,
		WorldDirection
	);

	// Ray 설정
	FVector Start = WorldLocation;
	FVector End = Start + WorldDirection * 10000.f;

	FHitResult Hit;
	FVector HitLocation = End;

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility))
	{
		HitLocation = Hit.Location; // 충돌한 위치
	}

	// 발사 방향 계산
	FVector ShootDirection = (HitLocation - MuzzleLocation).GetSafeNormal();

	// 총알 스폰
	ABulletActor* Bullet = GetWorld()->SpawnActor<ABulletActor>(
		BulletClass,
		MuzzleLocation,
		ShootDirection.Rotation()
	);

	if (Bullet)
	{
		Bullet->SetOwner(GetOwner());
		Bullet->ShootInDirection(ShootDirection, BulletSpeed);
	}
}

// Called when the game starts or when spawned
void AGunWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	
}

