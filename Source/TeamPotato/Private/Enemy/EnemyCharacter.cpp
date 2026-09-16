// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyCharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIController.h" 
#include "BrainComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/Enemy/EnemyHealthBarWidget.h"
#include "Subsystem/PoolingSubsystem.h"
#include "Item/PickupActor.h"
#include "Item/PickupHealthActor.h"
#include "Item/PickupStaminaActor.h"
#include "Item/PickupGoldActor.h"
#include "Combat/CombatAbilitySystemComponent.h"
#include "Combat/CombatFunctionLibrary.h"
#include "Engine/DamageEvents.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
    CombatAbilitySystem = CreateDefaultSubobject<UCombatAbilitySystemComponent>(TEXT("CombatAbilitySystem"));

    HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
    HealthBarWidgetComponent->SetupAttachment(GetMesh());

    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

    DamagePopupSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("DamagePopupSpawnPoint"));
    DamagePopupSpawnPoint->SetupAttachment(RootComponent);
}

UAbilitySystemComponent* AEnemyCharacter::GetAbilitySystemComponent() const
{
    return CombatAbilitySystem;
}

void AEnemyCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    CombatAbilitySystem->OnCombatHealthChanged.AddUObject(this, &AEnemyCharacter::HandleCombatHealthChanged);
    CombatAbilitySystem->OnCombatDamageReceived.AddUObject(this, &AEnemyCharacter::HandleCombatDamage);
    CombatAbilitySystem->OnCombatDeath.AddUObject(this, &AEnemyCharacter::OnDie);
    CombatAbilitySystem->InitializeCombat(MaxHealth, InvincibilityDuration);
}

void AEnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearAllTimersForObject(this);
    CombatAbilitySystem->OnCombatHealthChanged.RemoveAll(this);
    CombatAbilitySystem->OnCombatDamageReceived.RemoveAll(this);
    CombatAbilitySystem->OnCombatDeath.RemoveAll(this);
    Super::EndPlay(EndPlayReason);
}

float AEnemyCharacter::GetCombatHealth() const
{
    return CombatAbilitySystem->IsCombatInitialized() ? CombatAbilitySystem->GetHealth() : MaxHealth;
}

float AEnemyCharacter::GetCombatMaxHealth() const
{
    return CombatAbilitySystem->IsCombatInitialized() ? CombatAbilitySystem->GetMaxHealth() : MaxHealth;
}

void AEnemyCharacter::SetCombatMaxHealth(float NewMaxHealth)
{
    if (!HasAuthority() || !FMath::IsFinite(NewMaxHealth)) return;
    if (CombatAbilitySystem->IsCombatInitialized())
    {
        CombatAbilitySystem->AddMaxHealth(FMath::Max(1.0f, NewMaxHealth) - GetCombatMaxHealth());
    }
    else
    {
        MaxHealth = FMath::Max(1.0f, NewMaxHealth);
    }
}

void AEnemyCharacter::HandleCombatHealthChanged(float NewHealth, float NewMaxHealth)
{
    CurrentHealth = NewHealth;
    MaxHealth = NewMaxHealth;
    SetupHealthBarWidget();
}

void AEnemyCharacter::HandleCombatDamage(float ActualDamage, const FGameplayEffectContextHandle& Context)
{
    const APawn* DamageInstigator = Cast<APawn>(Context.GetOriginalInstigator());
    AActor::TakeDamage(ActualDamage, FDamageEvent(), DamageInstigator ? DamageInstigator->GetController() : nullptr, Context.GetEffectCauser());
    if (PoolingSubsystem && DamagePopupSpawnPoint)
    {
        PoolingSubsystem->GetPooledDamagePopupActor(ActualDamage, DamagePopupSpawnPoint->GetComponentLocation());
    }
}

void AEnemyCharacter::SetDropItemClasses(TSubclassOf<class APickupHealthActor> InHealthClass, TSubclassOf<class APickupStaminaActor> InStaminaClass, TSubclassOf<class APickupGoldActor> InGoldClass)
{
    HealthPickupClass = InHealthClass;
    StaminaPickupClass = InStaminaClass;
    GoldPickupClass = InGoldClass;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
    WieldWeapon();

    if (HealthBarWidgetComponent)
    {
        if (UUserWidget* Widget = HealthBarWidgetComponent->GetWidget())
        {
            HealthBarWidget = Cast<UEnemyHealthBarWidget>(Widget);
            SetupHealthBarWidget();
        }
    }

    // 풀링 서브시스템 참조 가져오기
    //PoolingSubsystem = GetGameInstance()->GetSubsystem<UPoolingSubsystem>();
    PoolingSubsystem = GetWorld()->GetSubsystem<UPoolingSubsystem>();

    // 주기적으로 체력바를 플레이어 쪽으로 회전시키는 타이머 설정
    FTimerHandle RotateTimerHandle;
    GetWorld()->GetTimerManager().SetTimer
    (
        RotateTimerHandle,
        this,
        &AEnemyCharacter::RotateHealthBarToViewport,
        0.01f,
        true
    );
}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    return UCombatFunctionLibrary::ApplyCombatDamage(this, DamageAmount, DamageCauser, EventInstigator);
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
    ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    //공격 전 위치랑 회전 설정
    if (Player)
    {
        FVector MyLoc = GetActorLocation();
        FVector TargetLoc = Player->GetActorLocation();

        MyLoc.Z = 0.0f;
        TargetLoc.Z = 0.0f;

        FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(MyLoc, TargetLoc);

        SetActorRotation(LookAtRot);

        if (GetController())
        {
            GetController()->SetControlRotation(LookAtRot);
        }
    }
    //적은 몽타주만 재생하고 데미지는 무기가 넣음
    if (AttackMontage)
    {
        PlayAnimMontage(AttackMontage);
    }
}

bool AEnemyCharacter::PlayerFocus(AActor* TargetActor, float DeltaTime, float TurnSpeed)
{
    if (!TargetActor || !IsValid(TargetActor))
    {
        return false;
    }
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
    if (bDeathHandled || !HasAuthority()) return;
    bDeathHandled = true;
    CombatAbilitySystem->MarkDead();
    // 죽으면 바로 위젯 컴포넌트 숨기기
    if (HealthBarWidget)
    {
        HealthBarWidgetComponent->SetVisibility(false);
    }

    if (OnDeath.IsBound())
    {
        OnDeath.Broadcast();
    }

    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        // 뇌(Behavior Tree)가 있다면 사고 정지. (더 이상 추적/공격 시도 안 함)
        if (AIController->GetBrainComponent())
        {
            AIController->GetBrainComponent()->StopLogic("Dead");
        }
        AIController->StopMovement(); // 이동 명령 즉시 취소
    }

    float AnimDuration = PlayAnimMontage(DeadMontage);

    SetLifeSpan(AnimDuration > 0.1f ? AnimDuration : 0.1f);
    if (CurrentWeapon)
    {
        CurrentWeapon->SetLifeSpan(AnimDuration > 0.1f ? AnimDuration : 0.1f);
    }
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCharacterMovement()->DisableMovement();

    EnemyItemDrop();
}

void AEnemyCharacter::SetupHealthBarWidget()
{
    if (HealthBarWidget)
    {
        HealthBarWidget->SetHealthPercent(GetCombatHealth() / FMath::Max(1.0f, GetCombatMaxHealth()));
    }
}

void AEnemyCharacter::RotateHealthBarToViewport()
{
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (!PlayerController || !HealthBarWidget) return;
    FRotator ViewportRotation = PlayerController->GetControlRotation();
    FRotator WidgetRotationForLookAtViewport = FRotator(0.0f, ViewportRotation.Yaw + 180.0f, 0.0f);

    if (PlayerController && HealthBarWidget)
    {
        // 위젯을 카메라를 바라보게 회전
        HealthBarWidgetComponent->SetWorldRotation(WidgetRotationForLookAtViewport);
    }
}

void AEnemyCharacter::EnemyItemDrop()
{
    UWorld* World = GetWorld();
    if (!World) return;

    float RandomChance = FMath::FRand();

    DropCount = FMath::RandRange(1, 4) * Elitemultiple * Bossmultiple;
    for (int32 i = 0; i < DropCount; i++)
    {
        FVector RandomOffset = FMath::VRand() * 50.0f;
        RandomOffset.Z = 50.0f;
        UE_LOG(LogTemp, Log, TEXT("DropItem"))
        World->SpawnActor<APickupActor>(GoldPickupClass, GetActorLocation() + RandomOffset, FRotator::ZeroRotator);
    }

    if (RandomChance <= 0.5f)
    {
        FVector RandomOffset = FMath::VRand() * 50.0f;
        RandomOffset.Z = 50.0f;
        DropCount = FMath::RandRange(1, 2);
        for (int32 i = 0; i < DropCount; i++)
        {
            World->SpawnActor<APickupActor>(StaminaPickupClass, GetActorLocation() + RandomOffset, FRotator::ZeroRotator);
        }
    }

    if (RandomChance <= 0.1f)
    {
        FVector RandomOffset = FMath::VRand() * 50.0f;
        RandomOffset.Z = 50.0f;
        World->SpawnActor<APickupActor>(HealthPickupClass, GetActorLocation() + RandomOffset, FRotator::ZeroRotator);
    }
}
