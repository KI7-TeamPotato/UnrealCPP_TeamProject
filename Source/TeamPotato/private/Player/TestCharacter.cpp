// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TestCharacter.h"
#include "Player/PlayerAnimation.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Item/Weapon/WeaponManagerActor.h"
#include "Item/Weapon/WeaponPickupActor.h"
#include "Item/Weapon/WeaponBoxActor.h"
#include "Component/WeaponComponent.h"
#include "Component/PlayerResource.h"
#include "Components/CapsuleComponent.h"
#include "Blueprint/UserWidget.h"
#include "IntetFace/Interactable.h"

// Sets default values
ATestCharacter::ATestCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//컨트롤러 방향으로 회전
	MovementComponent = GetCharacterMovement();
	MovementComponent->bUseControllerDesiredRotation = true;
	MovementComponent->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true;

	//스프링 암 생성
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = SpringArmLenght;
	SpringArm->SocketOffset = SpringArmOffset;
	SpringArm->bUsePawnControlRotation = true;		//컨트롤러를 따라 회전하도록 설정

	//카메라 생성
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArm);
	CameraComponent->SetRelativeRotation(CameraRotation);

	WeaponManager = CreateDefaultSubobject<AWeaponManagerActor>(TEXT("WeaponManager"));

	PlayerAnimation = CreateDefaultSubobject<UPlayerAnimation>(TEXT("PlayerAnimation"));

	ActivatedWeapon = EWeaponType::None;			//테스트용

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));

	ResourceManager = CreateDefaultSubobject<UPlayerResource>(TEXT("ResourceManager"));

    //데미지 받는 함수 바인딩
    OnTakeAnyDamage.AddDynamic(this, &ATestCharacter::TakeAnyDamage);

    ActivatedWeapon = EWeaponType::Gun;
}

// Called when the game starts or when spawned
void ATestCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetMesh())
	{
		AnimInstance = GetMesh()->GetAnimInstance();	// ABP 객체 가져오기
	}
	
    if (CrosshairWidgetClass)
    {
        CrosshairWidget = CreateWidget<UUserWidget>(
            GetWorld()->GetFirstPlayerController(),
            CrosshairWidgetClass
        );

        if (CrosshairWidget)
        {
            CrosshairWidget->AddToViewport();
            CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void ATestCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ATestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    /*if (bIsOnActing)
    {
        FRotator currentRotation = GetActorRotation();
        SetActorRotation(currentRotation);
    }*/
}

// Called to bind functionality to input
void ATestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (enhancedInputComponent)
	{
		enhancedInputComponent->BindAction(IA_FrontMove, ETriggerEvent::Triggered, this, &ATestCharacter::OnFrontMovementInput);
		enhancedInputComponent->BindAction(IA_FrontMove, ETriggerEvent::Completed, this, &ATestCharacter::OnFrontMovementComplete);
		enhancedInputComponent->BindAction(IA_SideMove, ETriggerEvent::Triggered, this, &ATestCharacter::OnSideMovementInput);
		enhancedInputComponent->BindAction(IA_SideMove, ETriggerEvent::Completed, this, &ATestCharacter::OnSideMovementComplete);
		enhancedInputComponent->BindAction(IA_HorizonSight, ETriggerEvent::Triggered, this, &ATestCharacter::OnHorizonSightInput);
		enhancedInputComponent->BindAction(IA_VerticalSight, ETriggerEvent::Triggered, this, &ATestCharacter::OnVerticalSightInput);
		enhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Started, this, &ATestCharacter::OnAttackInput);
		enhancedInputComponent->BindAction(IA_Roll, ETriggerEvent::Started, this, &ATestCharacter::OnRollInput);
		enhancedInputComponent->BindAction(IA_Interact, ETriggerEvent::Started, this, &ATestCharacter::OnInteract);
        enhancedInputComponent->BindAction(IA_WeaponSwap, ETriggerEvent::Started, this, &ATestCharacter::OnWeaponSwap);
	}
}

void ATestCharacter::SetPlayerActivatedWeapon(EWeaponType InActivatedWeapon)
{
    ActivatedWeapon = InActivatedWeapon;

    if (!CrosshairWidget) return;

    if (InActivatedWeapon == EWeaponType::Gun)
        CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
    else
        CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ATestCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

    if (OtherActor &&
        OtherActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
    {
        CurrentInteractTarget = OtherActor;
    }
}

void ATestCharacter::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

    if (OtherActor == CurrentInteractTarget)
    {
        CurrentInteractTarget = nullptr;
    }
}

void ATestCharacter::TakeAnyDamage(AActor* DamagedActor, float InDamage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    ResourceManager->PlayerTakeDamage(InDamage);
}


void ATestCharacter::KillPlayer()
{
	// 1. 캐릭터 이동 중지
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();

	// 2. 캡슐 충돌 끄기 
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 3. 메시를 Ragdoll 프로파일로
	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));

	// 4. 물리 활성화
	MeshComp->SetSimulatePhysics(true);

	// 5. 캡슐에서 분리 
	MeshComp->DetachFromComponent(
		FDetachmentTransformRules::KeepWorldTransform
	);
}

void ATestCharacter::InvincibleActivate()
{
	//콜리전 끔
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
}

void ATestCharacter::InvincibleDeactivate()
{
	//콜리전 켬
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ATestCharacter::PlaySwordAttackMontage()
{
	PlayAnimMontage(AttackMontage_Sword);
}

void ATestCharacter::PlayGunShootingMontage()
{
	PlayAnimMontage(AttackMontage_Gun);
}

void ATestCharacter::PlayDodgeMontage_Front_Sword()
{
	PlayAnimMontage(RollMontage_Sword);
}

void ATestCharacter::PlayDodgeMontage_Right_Sword()
{
    PlayAnimMontage(RightStepMontage_Sword);
}

void ATestCharacter::PlayDodgeMontage_Left_Sword()
{
    PlayAnimMontage(LeftStepMontage_Sword);
}

void ATestCharacter::PlayDodgeMontage_Back_Sword()
{
    PlayAnimMontage(BackStepMontage_Sword);
}

void ATestCharacter::PlayDodgeMontage_Front_Gun()
{
    PlayAnimMontage(RollMontage_Gun);
}

void ATestCharacter::PlayDodgeMontage_Back_Gun()
{
    SetAnimRootMotionIgnore();
    FVector playerDirection = -GetActorForwardVector();                         //플레이어 기준 뒷쪽 방향 벡터
    LaunchCharacter(playerDirection * LaunchPlayerPower, false, false);         //플레이어를 뒷쪽으로 이동
    PlayAnimMontage(BackStepMontage_Gun); 
}

void ATestCharacter::PlayDodgeMontage_Right_Gun()
{
    SetAnimRootMotionIgnore();
    FVector playerDirection = GetActorRightVector();                            //플레이어 기준 오른쪽 방향 벡터
    LaunchCharacter(playerDirection * LaunchPlayerPower, false, false);         //플레이어를 오른쪽으로 이동
    PlayAnimMontage(BackStepMontage_Gun);
}

void ATestCharacter::PlayDodgeMontage_Left_Gun()
{
    SetAnimRootMotionIgnore();
    FVector playerDirection = -GetActorRightVector();                           //플레이어 기준 왼쪽 방향 벡터
    LaunchCharacter(playerDirection * LaunchPlayerPower, false, false);         //플레이어를 왼쪽으로 이동
    PlayAnimMontage(BackStepMontage_Gun);
}


void ATestCharacter::OnMovementInput(const FInputActionValue& InValue)
{
	//FVector2D inputtedMovement = InValue.Get<FVector2D>();
	//FVector moveDirection(inputtedMovement.Y, inputtedMovement.X, 0.0f);
	//FQuat controlYawRotation = FQuat(FRotator(0, GetControlRotation().Yaw, 0));	// 컨트롤러의 Yaw회전을 따로 뽑아와서
	//moveDirection = controlYawRotation.RotateVector(moveDirection);	// 이동 방향에 적용


	//AddMovementInput(moveDirection);
}

void ATestCharacter::OnFrontMovementInput(const FInputActionValue& InValue)
{
    if(!bIsOnActing)
    {
        FrontBackMove = InValue.Get<float>();
        if (Controller && FrontBackMove != 0.0f)
        {
            FRotator controllerRotation = Controller->GetControlRotation();
            FRotator controllerYawRotation(0, controllerRotation.Yaw, 0);

            FVector Direction = FRotationMatrix(controllerYawRotation).GetUnitAxis(EAxis::X);
            AddMovementInput(Direction, FrontBackMove);
        }
    }
}


void ATestCharacter::OnSideMovementInput(const FInputActionValue& InValue)
{
    if (!bIsOnActing)
    {
        SideMove = InValue.Get<float>();
        if (Controller && SideMove != 0.0f)
        {
            FRotator controllerRotation = Controller->GetControlRotation();
            FRotator controllerYawRotation(0, controllerRotation.Yaw, 0);

            FVector Direction = FRotationMatrix(controllerYawRotation).GetUnitAxis(EAxis::Y);
            AddMovementInput(Direction, SideMove);
        }
    }
}

void ATestCharacter::OnFrontMovementComplete()
{
    FrontBackMove = 0.0f;
}

void ATestCharacter::OnSideMovementComplete()
{
    SideMove = 0.0f;
}

void ATestCharacter::OnHorizonSightInput(const FInputActionValue& InValue)
{
	AddControllerYawInput((InValue.Get<float>()) * HorizonMouseSensivility);	//마우스 움직이는 값 * 마우스 감도만큼 움직임
}

void ATestCharacter::OnVerticalSightInput(const FInputActionValue& InValue)
{
	SightDegree = ((InValue.Get<float>()) * VerticalMouseSensivility) * Reverse;		//마우스 움직이는 값 * 마우스 감도만큼 움직임
	PlayerVerticalDegree += SightDegree;

	if (PlayerVerticalDegree > MaxSightAngle)		//위로 넘어가지 않도록
	{
		PlayerVerticalDegree = MaxSightAngle;
		SightDegree = 0.0f;
	}
	else if (PlayerVerticalDegree < -MaxSightAngle)	//아래로 넘어가지 않도록
	{
		PlayerVerticalDegree = -MaxSightAngle;
		SightDegree = 0.0f;
	}
	
	AddControllerPitchInput(SightDegree);
}

void ATestCharacter::OnAttackInput()
{
	if(!bIsOnActing)
	{
		PlayerAnimation->PlayAttackAnimation();
		WeaponComponent->WeaponAttack();
	}
}

void ATestCharacter::OnInteract()
{
    if (!CurrentInteractTarget) return;

    if (CurrentInteractTarget->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
    {
        IInteractable::Execute_Interact(CurrentInteractTarget, this);
        CurrentInteractTarget = nullptr;
    }
}

void ATestCharacter::OnWeaponSwap()
{
    WeaponComponent->SwapWeapon();
}

void ATestCharacter::RotatePlayer(EMovingDirection TurnDirection)
{
    FRotator currentRotation = GetActorRotation();
    if (TurnDirection == EMovingDirection::Right)
    {
        currentRotation.Yaw += AnimRotateDegree;
    }
    else if(TurnDirection == EMovingDirection::Left)
    {
        currentRotation.Yaw -= AnimRotateDegree;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ATestCharacter::RotatePlayer | wrong direction"));
    }
    SetActorRotation(currentRotation);
    bIsOnActing = true;
}

void ATestCharacter::SetAnimRootMotionIgnore()
{
    AnimInstance->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
}

void ATestCharacter::SetAnimRootMotionFromMontage()
{
    AnimInstance->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
}


EMovingDirection ATestCharacter::GetLastInput()
{
    EMovingDirection playerDirection;

    if (FrontBackMove > 0)
    {
        if (SideMove > 0)
            playerDirection = EMovingDirection::FrontRight;
        else if (SideMove < 0)
            playerDirection = EMovingDirection::FrontLeft;
        else
            playerDirection = EMovingDirection::Front;
    }
    else if (FrontBackMove < 0)
    {
        if (SideMove > 0)
            playerDirection = EMovingDirection::BackRight;
        else if (SideMove < 0)
            playerDirection = EMovingDirection::BackLeft;
        else
            playerDirection = EMovingDirection::Back;
    }
    else
    {
        if (SideMove > 0)
            playerDirection = EMovingDirection::Right;
        else if (SideMove < 0)
            playerDirection = EMovingDirection::Left;
    }

    return playerDirection;
}

void ATestCharacter::SetOnActing(bool InActing)
{
    bIsOnActing = InActing;
    if(!bIsOnActing)
    {
        MovementComponent->bUseControllerDesiredRotation = true;
        MovementComponent->bOrientRotationToMovement = false;
        bUseControllerRotationYaw = true;
    }
}

void ATestCharacter::OnRollInput()
{
	//KillPlayer();				//테스트용으로 사망 연출 실행해봄
	if (!bIsOnActing)
	{
		/*if (ResourceManager->UseStamina(RollStamina))
		{
			PlayerAnimation->PlayRollMontage();
		}*/
        MovementComponent->bUseControllerDesiredRotation = false;
        MovementComponent->bOrientRotationToMovement = true;
        bUseControllerRotationYaw = false;
		PlayerAnimation->PlayRollMontage();
	}
}

