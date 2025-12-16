// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/TestWeapon.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"


ATestWeapon::ATestWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 콜리전 초기화
	WeaponCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	WeaponCollision->SetupAttachment(WeaponMesh);
	WeaponCollision->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

}

void ATestWeapon::BeginPlay()
{
	Super::BeginPlay();

	//ExecutePlayerAttack = CreateDefaultSubobject<APlayerAttack>(TEXT("PlayerAttackAction"));

	// 오버랩 델리게이트 이벤트 추가
	OnActorBeginOverlap.AddDynamic(this, &ATestWeapon::OnWeaponBeginOverlap);
}

void ATestWeapon::OnWeaponBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	UE_LOG(LogTemp, Log, TEXT("오버랩 : %s"), *OtherActor->GetName());
	DamageToTarget(OtherActor);
}

void ATestWeapon::DamageToTarget(AActor* InTarget)
{
	//UE_LOG(LogTemp, Log, TEXT("오버랩 : %s"),*OtherActor->GetName());
	float finalDamage = AttackDamage;
	AController* instigator = nullptr;

	UGameplayStatics::ApplyDamage(InTarget, finalDamage, instigator, this, DamageType);
}
