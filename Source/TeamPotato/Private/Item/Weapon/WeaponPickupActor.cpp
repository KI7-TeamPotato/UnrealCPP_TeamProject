// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/WeaponPickupActor.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Player/TestCharacter.h"
#include "Component/WeaponComponent.h"

AWeaponPickupActor::AWeaponPickupActor()
{
	// 위젯 컴포넌트 초기화
	InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
	InteractionWidget->SetupAttachment(RootComponent);
	InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionWidget->SetDrawSize(FVector2D(200, 50));
	InteractionWidget->SetVisibility(false);
}

void AWeaponPickupActor::OnPickup(AActor* InPlayer)
{
	ATestCharacter* player = Cast<ATestCharacter>(InPlayer);
	if (!player) return;

	UWeaponComponent* WeaponComponent = player->GetWeaponComponent();
	if (!WeaponComponent) return;

	// 플레이어의 웨폰 컴포넌트를 가져와 무기 장착 실행
	WeaponComponent->EquipWeapon(WeaponClass);

	Destroy();
}

void AWeaponPickupActor::BeginPlay()
{
	Super::BeginPlay();

	// 무기 아이템 오버랩 델리게이트 등록
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponPickupActor::OnBeginOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &AWeaponPickupActor::OnEndOverlap);
}

void AWeaponPickupActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 플레이어가 오버랩 시 위젯 출력
	if (OtherActor && OtherActor->IsA<ACharacter>())
	{
		InteractionWidget->SetVisibility(true);
	}
}

void AWeaponPickupActor::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 플레이어가 오버랩 종료 시 위젯 출력 종료
	ATestCharacter* Character = Cast<ATestCharacter>(OtherActor);
	if (!Character) return;

	InteractionWidget->SetVisibility(false);

	if (Character->PickupWeapon == this)
	{
		Character->PickupWeapon = nullptr;
	}
}
