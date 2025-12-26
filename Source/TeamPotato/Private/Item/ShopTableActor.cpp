// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ShopTableActor.h"
#include "Item/PickupActor.h"
#include "Data/WeaponPickupData.h"
#include "Data/WeaponDataAsset.h"
#include "Player/TestCharacter.h"
#include "Component/PlayerResource.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "UI/Player/ShopInteractWidget.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AShopTableActor::AShopTableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);

    // 스폰 포인트 초기화
    SpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
    SpawnPoint->SetupAttachment(Mesh);
    SpawnPoint->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));

    // 상호작용 범위 초기화
    SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    SphereCollision->InitSphereRadius(120.0f);
    SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    SphereCollision->SetupAttachment(Mesh);

    SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AShopTableActor::OnBeginOverlap);
    SphereCollision->OnComponentEndOverlap.AddDynamic(this, &AShopTableActor::OnEndOverlap);

    // 위젯 컴포넌트 초기화
    InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
    InteractionWidget->SetupAttachment(Mesh);
    InteractionWidget->SetWidgetClass(InteractionWidgetClass);
    InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
    InteractionWidget->SetDrawSize(FVector2D(200, 50));
    InteractionWidget->SetVisibility(false);
}

void AShopTableActor::Interact_Implementation(AActor* InTarget)
{
    BuyItem(InTarget);
}

// Called when the game starts or when spawned
void AShopTableActor::BeginPlay()
{
	Super::BeginPlay();
	
    // 블루프린트에서 지정 시 아이템 바로 스폰
    TSubclassOf<APickupActor> SpawnClass = SellItem;

    // 블루프린트에서 지정해주지 않으면 랜덤 무기 스폰(무기 테이블)
    if (!SpawnClass)
    {
        if (!WeaponDataTable) return;

        TArray<FWeaponPickupData*> Rows;
        WeaponDataTable->GetAllRows(TEXT("ShopWeapon"), Rows);
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

                SpawnClass = Row->WeaponData->PickupWeaponClass;
                break;
            }
        }
    }

    // 아이템 스폰
    FTransform SpawnTransform = SpawnPoint->GetComponentTransform();

    SpawnItem = GetWorld()->SpawnActor<APickupActor>(
        SpawnClass,
        SpawnTransform
    );
    if (SpawnItem)
    {
        SpawnItem->SetSellItem(true);
        Price = SpawnItem->GetPrice();
        if (UShopInteractWidget* Widget =
            Cast<UShopInteractWidget>(InteractionWidget->GetUserWidgetObject()))
        {
            Widget->SetPrice(Price);
        }
    }
}

void AShopTableActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->IsA<ATestCharacter>())
    {
        InteractionWidget->SetVisibility(true);
    }
}

void AShopTableActor::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor->IsA<ATestCharacter>())
    {
        InteractionWidget->SetVisibility(false);
    }
}

void AShopTableActor::BuyItem(AActor* InPlayer)
{
    ATestCharacter* player = Cast<ATestCharacter>(InPlayer);
    if (!player) return;

    UPlayerResource* Resource = player->GetResource();
    if (!Resource) return;

    if (Resource->GetCurrentGold() > SpawnItem->GetPrice())
    {
        if (BuySound)
            UGameplayStatics::PlaySound2D(this, BuySound);
        SpawnItem->SetSellItem(false);
        SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        SpawnItem->OnPickup(InPlayer);
        Resource->AddGold(-SpawnItem->GetPrice());
    }
    else
    {
        if (FailSound)
            UGameplayStatics::PlaySound2D(this, FailSound);
    }
}
