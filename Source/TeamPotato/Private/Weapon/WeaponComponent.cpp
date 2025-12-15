// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (Owner)
	{
		OwnerMesh = Owner->GetMesh();
	}
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	FTransform SocketTransform = OwnerMesh->GetSocketTransform("Weapon_R", RTS_World);
	FVector MuzzleLocation = SocketTransform.GetLocation();
	FRotator MuzzleRotation = SocketTransform.GetRotation().Rotator();

	// 화면 중심 좌표 계산
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	FVector2D ScreenCenter(ViewportSize.X / 2, ViewportSize.Y / 2);

	// 화면 중심에서 월드 방향 가져오기
	FVector WorldLocation, WorldDirection;
	UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(GetWorld(), 0), ScreenCenter, WorldLocation, WorldDirection);

	// Ray 설정
	FVector Start = WorldLocation;
	FVector End = Start + (WorldDirection * 10000); // Ray 길이
	FHitResult HitResult;

	FVector HitLocation;
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility))
	{
		HitLocation = HitResult.Location; // 충돌한 위치
	}
	else
	{
		HitLocation = End; // 충돌하지 않으면 Ray 끝점
	}

	// 발사 방향 계산
	FVector ShootDirection = (HitLocation - MuzzleLocation).GetSafeNormal();

}

void UWeaponComponent::FireWeapon()
{
}


// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

