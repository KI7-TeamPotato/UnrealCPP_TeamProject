// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/LaserGunWeaponActor.h"
#include "Component/WeaponComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/TestCharacter.h"

ALaserGunWeaponActor::ALaserGunWeaponActor()
{
    // 무기 타입 설정
    WeaponType = EWeaponType::Gun;
    AttackDamage = 10.0f;
    AttackCost = 1.0f;
}

void ALaserGunWeaponActor::Attack(ATestCharacter* OwningPlayer)
{
    Super::Attack(OwningPlayer);

    // 오류 검사
    if (!OwnerWeaponComponent || !WeaponMesh) return;

    OwningPlayer = Cast<ATestCharacter>(GetOwner());
    if (!OwningPlayer) return;

    APlayerController* PC = Cast<APlayerController>(OwningPlayer->GetController());
    if (!PC) return;

    // 총구 위치 계산
    FTransform SocketTransform = WeaponMesh->GetSocketTransform("FirePoint", RTS_World);
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

    if (AttackSound)
        UGameplayStatics::PlaySound2D(this, AttackSound);

    // 총알 스폰
    AActor* Laser = GetWorld()->SpawnActor<AActor>(
        LaserClass,
        MuzzleLocation,
        ShootDirection.Rotation()
    );
}
