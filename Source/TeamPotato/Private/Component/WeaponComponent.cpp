// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/WeaponComponent.h"
#include "Player/TestCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Item/Weapon/GunWeaponActor.h"
#include "Data/WeaponDataAsset.h"
#include "Subsystem/MVVMSubsystem.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

    if (UMVVMSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UMVVMSubsystem>())
    {
        Subsystem->RegisterWeaponComp(this);
    }

	Owner = Cast<ATestCharacter>(GetOwner());
}

void UWeaponComponent::WeaponAttack()
{
	if (!CurrentWeapon) return;

	if (CurrentWeapon)
	{
		CurrentWeapon->Attack(Owner);
	}
}

void UWeaponComponent::EquipCurrentWeapon(AWeaponBase* InWeapon)
{
    UE_LOG(LogTemp, Log, TEXT("EquipCurrentWeapon called"));

    if (!InWeapon || !Owner) return;

    CurrentWeapon = InWeapon;

    Owner->SetPlayerActivatedWeapon(CurrentWeapon->GetWeaponType());
    CurrentWeapon->SetActorHiddenInGame(false);
    CurrentWeapon->SetActorEnableCollision(true);
    CurrentWeapon->SetOwner(Owner);
    CurrentWeapon->SetOwnerComponent(this);

    if (CurrentWeapon->GetWeaponType() == EWeaponType::Sword)
    {
        CurrentWeapon->AttachToComponent(
            Owner->GetMesh(),
            FAttachmentTransformRules::SnapToTargetIncludingScale,
            TEXT("Weapon_Sword")
        );
    }
    else if (CurrentWeapon->GetWeaponType() == EWeaponType::Gun)
    {
        CurrentWeapon->AttachToComponent(
            Owner->GetMesh(),
            FAttachmentTransformRules::SnapToTargetIncludingScale,
            TEXT("Weapon_Gun")
        );
    }

    // 메인 무기 변경 브로드캐스트
    BroadcastMainWeaponChanged();
}

void UWeaponComponent::EquipSubWeapon(AWeaponBase* InWeapon)
{
    if (!InWeapon) return;

    SubWeapon = InWeapon;
    SubWeapon->SetActorHiddenInGame(true);
    SubWeapon->SetActorEnableCollision(false);

    // 서브 무기 변경 브로드캐스트
    BroadcastSubWeaponChanged();
}

void UWeaponComponent::PickupWeapon(UWeaponDataAsset* WeaponData)
{
    if (!Owner || !WeaponData || !WeaponData->WeaponClass) return;

    AWeaponBase* NewWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponData->WeaponClass);
    if (!NewWeapon) return;

    NewWeapon->InitializeFromData(WeaponData);

    // 현재 무기 없음
    if (!CurrentWeapon)
    {
        EquipCurrentWeapon(NewWeapon);
        return;
    }

    // 현재 무기 있음, 서브 무기 없음
    if (!SubWeapon)
    {
        EquipSubWeapon(NewWeapon);
        SubWeapon->SetOwner(Owner);
        return;
    }

    // 둘 다 있음

    AWeaponBase* OldWeapon = CurrentWeapon;
    
    // 새 무기 장착
    EquipCurrentWeapon(NewWeapon);
    // 기존 픽업 무기 드롭
    SpawnPickupWeapon(OldWeapon->GetWeaponData());
    // 기존 장착 무기 파괴
    OldWeapon->Destroy();
}

void UWeaponComponent::SwapWeapon()
{
    if (!CurrentWeapon || !SubWeapon) return;

    AWeaponBase* Temp = CurrentWeapon;

    EquipCurrentWeapon(SubWeapon);

    EquipSubWeapon(Temp);
}

void UWeaponComponent::SpawnPickupWeapon(UWeaponDataAsset* WeaponData)
{
    if (!WeaponData || !WeaponData->PickupWeaponClass || !Owner) return;

    FVector SpawnLocation = Owner->GetActorLocation() + Owner->GetActorForwardVector() * 100.f;
    FRotator SpawnRotation = FRotator::ZeroRotator;

    AWeaponPickupActor* Pickup = GetWorld()->SpawnActor<AWeaponPickupActor>(
        WeaponData->PickupWeaponClass,
        SpawnLocation,
        SpawnRotation
    );

    if (Pickup)
    {
        Pickup->SetWeaponData(WeaponData);
    }
}

EWeaponType UWeaponComponent::GetCurrentWeaponType() const
{
	return CurrentWeapon ? CurrentWeapon->GetWeaponType() : EWeaponType::None;
}


void UWeaponComponent::BroadcastMainWeaponChanged()
{
    if (OnMainWeaponChanged.IsBound())
    {
        OnMainWeaponChanged.Broadcast(CurrentWeapon->GetWeaponData());
    }
}

void UWeaponComponent::BroadcastSubWeaponChanged()
{
    if (OnSubWeaponChanged.IsBound())
    {
        OnSubWeaponChanged.Broadcast(SubWeapon->GetWeaponData());
    }
}