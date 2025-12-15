// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupActor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"

// Sets default values
APickupActor::APickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 아이템 획득 범위 초기화
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SphereCollision->InitSphereRadius(120.0f);
	SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = SphereCollision;

	// 아이템 메쉬 초기화
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
	Super::BeginPlay();

	// 오버랩 델리게이트 등록
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &APickupActor::OnOverlap);
	
}

void APickupActor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 오버랩 시 아이템 종류 확인
	if (!ItemInteraction())
	{
		// 무기가 아닐 경우 아이템 획득 함수 실행
		if (OtherActor && OtherActor->IsA<ACharacter>())
		{
			OnPickup(OtherActor);
		}
	}
}

void APickupActor::OnPickup(AActor* InPlayer)
{
	// 무기 외의 아이템 획득 시 사용(골드 혹은 포션 액터에서 override)
	Destroy();
}
