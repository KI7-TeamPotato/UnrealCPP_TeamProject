// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterSelectActor.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "UI/Player/ShopInteractWidget.h"
#include "Player/TestCharacter.h"
#include "UI/CharacterSelect/CharacterSelectMenu.h"

// Sets default values
ACharacterSelectActor::ACharacterSelectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(Mesh);
    // 상호작용 범위 초기화
    SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    SphereCollision->InitSphereRadius(150.0f);
    SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    SphereCollision->SetupAttachment(Mesh);
    SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ACharacterSelectActor::OnOverlap);
    SphereCollision->OnComponentEndOverlap.AddDynamic(this, &ACharacterSelectActor::OnEndOverlap);
    // 위젯 컴포넌트 초기화
    InteractionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidget"));
    InteractionWidget->SetupAttachment(Mesh);
    InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
    InteractionWidget->SetDrawSize(FVector2D(200, 50));
    InteractionWidget->SetVisibility(false);
}

void ACharacterSelectActor::Interact_Implementation(AActor* InTarget)
{
    SelectCharacter(InTarget);
}

// Called when the game starts or when spawned
void ACharacterSelectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACharacterSelectActor::SelectCharacter(AActor* InPlayer)
{
    // 캐릭터 선택 UI 표시 로직 추가
    if (CharacterSelectMenuClass)
    {
        CharacterSelectMenuInstance = NewObject<UCharacterSelectMenu>(this, CharacterSelectMenuClass);
        if (CharacterSelectMenuInstance)
        {
            CharacterSelectMenuInstance->AddToViewport();
        }
    }
}

// Called every frame
void ACharacterSelectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACharacterSelectActor::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if(OtherActor && OtherActor->IsA<ATestCharacter>())
    {
        InteractionWidget->SetVisibility(true);
    }
}

void ACharacterSelectActor::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor->IsA<ATestCharacter>())
    {
        InteractionWidget->SetVisibility(false);
    }
}