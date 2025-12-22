// EnemyWeapon.cpp

#include "TeamPotato/Public/Enemy/EnemyAxe.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h" // 무브먼트 제어용

AEnemyAxe::AEnemyAxe()
{
    PrimaryActorTick.bCanEverTick = true; // Tick을 켜야 날아갑니다.

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionBox;
}

void AEnemyAxe::BeginPlay()
{
    Super::BeginPlay();

    // 오버랩 바인딩
    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyAxe::OnBoxOverlap);
}

// 1. 던지기 시작 (적이 호출)
void AEnemyAxe::ThrowAt(FVector TargetLocation)
{
    WeaponOwner = GetOwner(); // 던진 사람 기억

    FVector StartLocation = WeaponOwner->GetActorLocation();
    FVector Direction = (TargetLocation - StartLocation).GetSafeNormal();
    float OvershootDistance = 500.0f;
    ThrowTargetLocation = TargetLocation + (Direction * OvershootDistance);
    CurrentState = EWeaponState::Thrown;

    // 주인에게서 분리 (손에서 떠남)
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    SetActorRotation(Direction.Rotation());
    AddActorLocalRotation(FRotator(45.0f, 180.0f, 0.0f));

    // 충돌 켜기
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

// 2. 이동 로직 (핵심)
void AEnemyAxe::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 손에 있으면 아무것도 안 함
    if (CurrentState == EWeaponState::Idle || !WeaponOwner) return;

    FVector CurrentLoc = GetActorLocation();

    // [상태 1: 날아가는 중]
    if (CurrentState == EWeaponState::Thrown)
    {
        // 목표 지점으로 이동 (VInterpConstantTo는 등속 운동)
        FVector NewLoc = FMath::VInterpConstantTo(CurrentLoc, ThrowTargetLocation, DeltaTime, ThrowSpeed);
        SetActorLocation(NewLoc);

        // 목표에 거의 도착했으면 -> 돌아오기 시작
        if (FVector::DistSquared(CurrentLoc, ThrowTargetLocation) < 100.0f) // 거리 체크
        {
            CurrentState = EWeaponState::Returning;
        }
    }
    // [상태 2: 돌아오는 중]
    else if (CurrentState == EWeaponState::Returning)
    {
        // 주인(적)의 위치로 이동
        FVector OwnerLoc = WeaponOwner->GetActorLocation();
        FVector NewLoc = FMath::VInterpConstantTo(CurrentLoc, OwnerLoc, DeltaTime, ReturnSpeed);
        SetActorLocation(NewLoc);

        // **플레이어를 잡았다면 같이 이동 (끌고 오기)**
        if (CaughtPlayer)
        {
            // 플레이어 위치를 무기 위치로 강제 설정 (Sweep을 켜서 벽뚫기 방지)
            CaughtPlayer->SetActorLocation(NewLoc, true);
        }

        // 주인에게 도착했으면 -> 장착 및 초기화
        if (FVector::DistSquared(CurrentLoc, OwnerLoc) < 2500.0f) // 약 50cm 이내
        {
            // 다시 장착
            AttachToComponent(WeaponOwner->FindComponentByClass<USkeletalMeshComponent>(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("RHandSoket"));

            // 상태 초기화
            CurrentState = EWeaponState::Idle;
            CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

            // 잡혀있던 플레이어 해방
            if (CaughtPlayer)
            {
                if (UCharacterMovementComponent* CMC = CaughtPlayer->GetCharacterMovement())
                {
                    CMC->SetMovementMode(MOVE_Walking); // 걷기 상태 복구
                }
                CaughtPlayer = nullptr;
            }
        }
    }
}

// 3. 충돌 처리 (플레이어 피격 시)
void AEnemyAxe::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 던져진 상태가 아니거나, 주인(적)이면 무시
    if (CurrentState != EWeaponState::Thrown || OtherActor == WeaponOwner) return;

    // 플레이어인지 확인 (Character 태그나 캐스팅 사용)
    ACharacter* HitCharacter = Cast<ACharacter>(OtherActor);
    if (HitCharacter)
    {
        // 데미지 주기
        UGameplayStatics::ApplyDamage(HitCharacter, 20.0f, GetInstigatorController(), this, UDamageType::StaticClass());

        // **[핵심] 플레이어를 잡음!**
        CaughtPlayer = HitCharacter;

        // 플레이어가 못 움직이게 만듦 (끌려와야 하니까)
        if (UCharacterMovementComponent* CMC = CaughtPlayer->GetCharacterMovement())
        {
            CMC->DisableMovement(); // 혹은 MOVE_Flying으로 변경
        }

        // 즉시 복귀 모드로 전환
        CurrentState = EWeaponState::Returning;
    }
    // 벽에 부딪히면? (선택 사항: 즉시 돌아오기)
    else
    {
        CurrentState = EWeaponState::Returning;
    }
}