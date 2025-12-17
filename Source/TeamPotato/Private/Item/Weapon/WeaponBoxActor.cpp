// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/WeaponBoxActor.h"
#include "Item/Weapon/WeaponPickupActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Player/TestCharacter.h"
#include "Data/WeaponDataAsset.h"

// Sets default values
AWeaponBoxActor::AWeaponBoxActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);

    // 상호작용 범위 초기화
    SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    SphereCollision->InitSphereRadius(120.0f);
    SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    SphereCollision->SetupAttachment(Mesh);

    // 아이템 스폰 위치 초기화
    SpawnLocation = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnLocation"));
    SpawnLocation->SetupAttachment(Mesh);
    SpawnLocation->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));

    // 위젯 컴포넌트 초기화
    InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
    InteractionWidget->SetupAttachment(Mesh);
    InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
    InteractionWidget->SetDrawSize(FVector2D(200, 50));
    InteractionWidget->SetVisibility(false);


    SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AWeaponBoxActor::OnBeginOverlap);
    SphereCollision->OnComponentEndOverlap.AddDynamic(this, &AWeaponBoxActor::OnEndOverlap);
    OnBoxOpen.AddDynamic(this, &AWeaponBoxActor::SpawnRandomWeapon);
}

void AWeaponBoxActor::Interact_Implementation(AActor* InTarget)
{
    PlayOpenAnimation();
}

// Called when the game starts or when spawned
void AWeaponBoxActor::BeginPlay()
{
	Super::BeginPlay();

}

void AWeaponBoxActor::SpawnRandomWeapon()
{
    if (!WeaponDataTable) return;

    TArray<FWeaponPickupData*> Rows;
    WeaponDataTable->GetAllRows(TEXT("WeaponBox"), Rows);
    if (Rows.Num() == 0) return;

    // 가중치 합산
    float TotalWeight = 0.0f;
    for (auto Row : Rows)
    {
        TotalWeight += Row->WeaponWeight;
    }

    // 랜덤 선택
    float Random = FMath::FRandRange(0.0f, TotalWeight);
    float Pick = 0.0f;

    for (auto Row : Rows)
    {
        Pick += Row->WeaponWeight;
        if (Random <= Pick)
        {
            if (!Row->WeaponData) return;

            SpawnPickup(Row->WeaponData);
            return;
        }
    }
}

void AWeaponBoxActor::SpawnPickup(UWeaponDataAsset* InWeaponData)
{
    if (!InWeaponData || !InWeaponData->PickupWeaponClass) return;

    AWeaponPickupActor* Pickup =
        GetWorld()->SpawnActor<AWeaponPickupActor>(
            InWeaponData->PickupWeaponClass,
            SpawnLocation->GetComponentTransform()
        );

    if (Pickup)
    {
        Pickup->SetWeaponData(InWeaponData);
    }
}

void AWeaponBoxActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->IsA<ATestCharacter>())
    {
        InteractionWidget->SetVisibility(true);
    }
}

void AWeaponBoxActor::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor->IsA<ATestCharacter>())
    {
        InteractionWidget->SetVisibility(false);
    }
}

void AWeaponBoxActor::BoxOpen()
{
    if (bOpened) return;

    bOpened = true;

    // 상자 열림 상태 처리
    InteractionWidget->SetVisibility(false);
    SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    OnBoxOpen.Broadcast();
}

void AWeaponBoxActor::PlayOpenAnimation()
{
    if (!Mesh || !OpenMontage)
    {
        BoxOpen();
        return;
    }

    UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
    if (!AnimInstance) return;

    AnimInstance->Montage_Play(OpenMontage);
}

void AWeaponBoxActor::OnSpawnWeaponNotify()
{
    // 실제 스폰 로직
    BoxOpen();
}
