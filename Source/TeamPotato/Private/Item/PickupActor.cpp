// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/PickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"

// Sets default values
APickupActor::APickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 아이템 획득 범위 초기화
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SphereCollision->InitSphereRadius(120.0f);
	SphereCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = SphereCollision;

	// 아이템 메쉬 초기화
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 타임라인 초기화
    PickupTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("PickupTimeline"));
}

void APickupActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    Mesh->AddWorldRotation(FRotator(0, RotateSpeed * DeltaTime, 0));
}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
    Super::BeginPlay();

    // 오버랩 델리게이트 등록
    SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &APickupActor::OnOverlap);

    if (PickupTimeline)
    {
        if (DistanceCurve)
        {
            FOnTimelineFloat UpdateDelegate;
            UpdateDelegate.BindUFunction(this, FName("OnTimeLineUpdate"));
            PickupTimeline->AddInterpFloat(DistanceCurve, UpdateDelegate);

            FOnTimelineEvent FinishDelegate;
            FinishDelegate.BindUFunction(this, FName("OnTimelineFinished"));
            PickupTimeline->SetTimelineFinishedFunc(FinishDelegate);
        }
        PickupTimeline->SetPlayRate(1 / Duration);
    }

    FTimerManager& timerManager = GetWorldTimerManager();
    timerManager.ClearTimer(PickupableTimer);

    timerManager.SetTimer(
        PickupableTimer,
        [this]() {
            if (bIsSell) return;
            SphereCollision->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
        },
        PickupableTime, false);
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
    if (bPickedUp) return;
    bPickedUp = true;

	// 무기 외의 아이템 획득 시 사용(골드 혹은 포션 액터에서 override)
    PickupOwner = InPlayer;
    PickupStartLocation = Mesh->GetRelativeLocation() + GetActorLocation();	// Mesh의 월드 위치

    SetActorEnableCollision(false);		// 액터와 액터가 포함하는 모든 컴포넌트의 충돌 정지
    Mesh->SetSimulatePhysics(false);// 바닥으로 가라앉는 것 방지

    if (PickupTimeline)
    {
        PickupTimeline->PlayFromStart();	// 타임라인 시작
    }
}

void APickupActor::SetSellItem(bool IsSell)
{
    bIsSell = IsSell;

    if (IsSell)
    {
        SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

int32 APickupActor::GetPrice() const
{
    return ItemPrice;
}

void APickupActor::OnTimeLineUpdate(float Value)
{
    // 타임라인 진행 시간(0~1)
    float currentTime = PickupTimeline->GetPlaybackPosition();
    //UE_LOG(LogTemp, Log, TEXT("Timeline : %.2f"), currentTime);

    // 커브의 현재 값을 받아오기
    float distanceValue = Value;
    float heightValue = HeightCurve ? HeightCurve->GetFloatValue(currentTime) : 0.0f;
    float scaleValue = ScaleCurve ? ScaleCurve->GetFloatValue(currentTime) : 1.0f;

    // 커브값을 기준으로 새 위치와 스케일 계산
    FVector NewLocation = FMath::Lerp(PickupStartLocation, PickupOwner.Get()->GetActorLocation(), distanceValue);
    NewLocation += FVector::UpVector * heightValue * PickupHeight;
    Mesh->SetWorldLocation(NewLocation);

    FVector NewScale = FVector::One() * scaleValue;
    Mesh->SetRelativeScale3D(NewScale);
}

void APickupActor::OnTimelineFinished()
{
    UseItem(PickupOwner.Get());
    Destroy();
}
