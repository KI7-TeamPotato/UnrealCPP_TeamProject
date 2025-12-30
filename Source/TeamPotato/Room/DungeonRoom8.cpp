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
    if (ShopTable_HP)
    {
        HPPickUpInstance = this->GetWorld()->SpawnActor<AShopTableActor>(
            ShopTable_HP,
            ShopTableSpawnPoint_HP->GetComponentTransform()
        );
    }

    if (ShopTable_Energy)
    {
        EnergyPickUPInstance = this->GetWorld()->SpawnActor<AShopTableActor>(
            ShopTable_Energy,
            ShopTableSpawnPoint_Energy->GetComponentTransform()
        );
    }

    if (ShopTable_Weapon)
    {
        WeaponPickupleInstance = this->GetWorld()->SpawnActor<AShopTableActor>(
            ShopTable_Weapon,
            ShopTableSpawnPoint_Weapon->GetComponentTransform()
        );
    }
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
    if (IsValid(HPPickUpInstance))
    {
        HPPickUpInstance->Destroy();
    }

    if (IsValid(EnergyPickUPInstance))
    {
        EnergyPickUPInstance->Destroy();
    }

    if (IsValid(WeaponPickupleInstance))
    {
        WeaponPickupleInstance->Destroy();
    }

    Super::EndPlay(EndPlayReason);
}
