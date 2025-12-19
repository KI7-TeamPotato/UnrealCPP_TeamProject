// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonRoom8.h"
#include "TeamPotato/Logic/DungeonGanarator.h"
#include "Item/ShopTableActor.h"

ADungeonRoom8::ADungeonRoom8()
{
    ShopTableSpawnPoint_HP = CreateDefaultSubobject<UArrowComponent>(TEXT("ShopTableSpawnPointHP"));
    ShopTableSpawnPoint_Energy = CreateDefaultSubobject<UArrowComponent>(TEXT("ShopTableSpawnPointEnergy"));
    ShopTableSpawnPoint_Weapon = CreateDefaultSubobject<UArrowComponent>(TEXT("ShopTableSpawnPointWeapon"));

    ShopTableSpawnPoint_HP->SetupAttachment(OtherArchitecture);
    ShopTableSpawnPoint_Energy->SetupAttachment(OtherArchitecture);
    ShopTableSpawnPoint_Weapon->SetupAttachment(OtherArchitecture);

}

void ADungeonRoom8::SpawnShopTable()
{
    AShopTableActor* HPPostionTable = this->GetWorld()->SpawnActor<AShopTableActor>(ShopTable_HP);
    HPPostionTable->SetActorLocation(ShopTableSpawnPoint_HP->GetComponentLocation());
    HPPostionTable->SetActorRotation(ShopTableSpawnPoint_HP->GetComponentRotation());

    AShopTableActor* EnergyPostionTable = this->GetWorld()->SpawnActor<AShopTableActor>(ShopTable_Energy);
    EnergyPostionTable->SetActorLocation(ShopTableSpawnPoint_Energy->GetComponentLocation());
    EnergyPostionTable->SetActorRotation(ShopTableSpawnPoint_Energy->GetComponentRotation());

    AShopTableActor* WeaponTable = this->GetWorld()->SpawnActor<AShopTableActor>(ShopTable_Weapon);
    WeaponTable->SetActorLocation(ShopTableSpawnPoint_Weapon->GetComponentLocation());
    WeaponTable->SetActorRotation(ShopTableSpawnPoint_Weapon->GetComponentRotation());
}

void ADungeonRoom8::BeginPlay()
{
    Super::BeginPlay();

    AActor* FoundGenerator = UGameplayStatics::GetActorOfClass(GetWorld(), ADungeonGanarator::StaticClass());
    ADungeonGanarator* Generator = Cast<ADungeonGanarator>(FoundGenerator);

    if (Generator)
    {
        // 제너레이터의 종료 델리게이트에 내 함수 바인딩
        Generator->EndedCreate.AddDynamic(this, &ADungeonRoom8::SpawnShopTable);
    }
}

void ADungeonRoom8::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (ShopTableInstance && IsValid(ShopTableInstance))
    {
        ShopTableInstance->Destroy();
    }
    Super::EndPlay(EndPlayReason);
}
