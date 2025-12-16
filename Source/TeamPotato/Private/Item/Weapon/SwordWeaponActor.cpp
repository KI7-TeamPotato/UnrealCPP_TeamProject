// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/SwordWeaponActor.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

ASwordWeaponActor::ASwordWeaponActor()
{
	// 콜리전 초기화
	WeaponCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	WeaponCollision->SetupAttachment(WeaponMesh);
	WeaponCollision->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	// 무기 타입 설정
	WeaponType = EWeaponType::Sword;
}


void ASwordWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	// 오버랩 델리게이트 이벤트 추가
	OnActorBeginOverlap.AddDynamic(this, &ASwordWeaponActor::OnWeaponBeginOverlap);
}

void ASwordWeaponActor::OnWeaponBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	UE_LOG(LogTemp, Log, TEXT("오버랩 : %s"), *OtherActor->GetName());
	DamageToTarget(OtherActor);
}

void ASwordWeaponActor::DamageToTarget(AActor* InTarget)
{
	//UE_LOG(LogTemp, Log, TEXT("오버랩 : %s"),*OtherActor->GetName());
	float finalDamage = AttackDamage;
	AController* instigator = nullptr;

	UGameplayStatics::ApplyDamage(InTarget, finalDamage, instigator, this, DamageType);
}