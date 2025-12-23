// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/SwordWeaponActor.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"

ASwordWeaponActor::ASwordWeaponActor()
{
	// 콜리전 초기화
	WeaponCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	WeaponCollision->SetupAttachment(WeaponMesh);
	WeaponCollision->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
    WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 트레일 초기화
    TrailEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Trail"));
    TrailEffect->SetupAttachment(WeaponMesh);
    TrailEffect->bAutoActivate = false;

	// 무기 타입 설정
	WeaponType = EWeaponType::Sword;
    AttackDamage = 0.0f;
    AttackCost = 0.0f;
}

void ASwordWeaponActor::BeginAttack()
{
    WeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

    if (TrailEffect)
    {
        TrailEffect->Activate();
    }
}

void ASwordWeaponActor::EndAttack()
{
    WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (TrailEffect)
    {
        TrailEffect->Deactivate();
    }
}


void ASwordWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	// 오버랩 델리게이트 이벤트 추가
	OnActorBeginOverlap.AddDynamic(this, &ASwordWeaponActor::OnWeaponBeginOverlap);
}

void ASwordWeaponActor::OnWeaponBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    if (OtherActor == GetOwner())
        return;

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