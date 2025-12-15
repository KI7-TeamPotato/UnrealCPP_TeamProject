// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyCharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
    CurrentHealth = MaxHealth;
    UE_LOG(LogTemp, Warning, TEXT("Enemy Spawned! HP: %f / %f"), CurrentHealth, MaxHealth);
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
    WieldWeapon();

}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // 1. 부모 클래스의 기본 로직 실행 (필수)
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // 데미지가 0 이하거나 이미 죽었다면 무시
    if (ActualDamage <= 0.0f || CurrentHealth <= 0.0f)
    {
        return 0.0f;
    }

    // 2. 체력 감소
    CurrentHealth -= ActualDamage;

    // 로그로 확인 (디버깅용)
    UE_LOG(LogTemp, Warning, TEXT("[%s] Took Damage: %f, HP: %f"), *GetName(), ActualDamage, CurrentHealth);

    // 3. 사망 체크
    if (CurrentHealth <= 0.0f)
    {
        OnDie();
    }

    return ActualDamage;
}

void AEnemyCharacter::WieldWeapon()
{
    //무기 없으면 리턴
    if (bIsWieldingWeapon || !WeaponClass) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = this;

    // 무기 스폰
    CurrentWeapon = GetWorld()->SpawnActor<AActor>(WeaponClass, GetActorTransform(), SpawnParams);

    if (CurrentWeapon)
    {
        // 소켓에 부착
        CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
        bIsWieldingWeapon = true;
    }
}

void AEnemyCharacter::DefaultAttack()
{
    if (AttackMontage)
    {
        PlayAnimMontage(AttackMontage);//아직은 몽타주만
    }
}

bool AEnemyCharacter::PlayerFocus(AActor* TargetActor, float DeltaTime, float TurnSpeed)
{
    FVector Start = GetActorLocation();
    FVector End = TargetActor->GetActorLocation();
    FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(Start, End);

    FRotator CurrentRot = GetActorRotation();

    FRotator NewRot = FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, TurnSpeed);

    SetActorRotation(FRotator(CurrentRot.Pitch, NewRot.Yaw, CurrentRot.Roll));

    float DeltaYaw = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRot.Yaw, TargetRot.Yaw));

    return DeltaYaw < 5.0f;
}

void AEnemyCharacter::UpdateMovementSpeed(float NewSpeed)
{
    GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}


float AEnemyCharacter::SetMovementSpeed_Implementation(EEnemySpeed State)
{
    float TargetSpeed = 0.0f;

    //EEnemySpeed에 따라서 속도 조절
    switch (State)
    {
    case EEnemySpeed::Idle:
        TargetSpeed = 0.0f;
        break;
    case EEnemySpeed::Walking:
        TargetSpeed = 200.0f;
        break;
    case EEnemySpeed::Jumping:
        TargetSpeed = 300.0f;
        break;
    case EEnemySpeed::Sprint:
        TargetSpeed = 500.0f;
        break;
    default:
        TargetSpeed = 100.0f; // 기본값
        break;
    }

    // 실제 이동 속도 적용
    GetCharacterMovement()->MaxWalkSpeed = TargetSpeed;

    // 변경된 속도 반환
    return TargetSpeed;
}

void AEnemyCharacter::OnDie()
{
    if (OnDeath.IsBound())
    {
        OnDeath.Broadcast();
    }

    if (CurrentWeapon)
    {
        CurrentWeapon->SetLifeSpan(3.0f);
    }

    GetCharacterMovement()->DisableMovement();
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DetachFromControllerPendingDestroy();
    SetLifeSpan(2.0f);
}

