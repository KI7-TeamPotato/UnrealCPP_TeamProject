// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TestCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Weapon/WeaponManagerActor.h"
#include "Weapon/WeaponComponent.h"
#include "Weapon/WeaponPickupActor.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/PlayerAnimation.h"

// Sets default values
ATestCharacter::ATestCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//컨트롤러 방향으로 회전
	MovementComponent = GetCharacterMovement();
	MovementComponent->bUseControllerDesiredRotation = false;
	MovementComponent->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

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

	ActivatedWeapon = EWeaponType::Sword;			//테스트용

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));
}

// Called when the game starts or when spawned
void ATestCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetMesh())
	{
		AnimInstance = GetMesh()->GetAnimInstance();	// ABP 객체 가져오기
	}
	
}

// Called every frame
void ATestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATestCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (enhancedInputComponent)
	{
		enhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ATestCharacter::OnMovementInput);
		enhancedInputComponent->BindAction(IA_HorizonSight, ETriggerEvent::Triggered, this, &ATestCharacter::OnHorizonSightInput);
		enhancedInputComponent->BindAction(IA_VerticalSight, ETriggerEvent::Triggered, this, &ATestCharacter::OnVerticalSightInput);
		enhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Started, this, &ATestCharacter::OnAttackInput);
		enhancedInputComponent->BindAction(IA_Roll, ETriggerEvent::Started, this, &ATestCharacter::OnRollInput);
		enhancedInputComponent->BindAction(IA_Interact, ETriggerEvent::Started, this, &ATestCharacter::OnInteract);
	}
}

void ATestCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	AWeaponPickupActor* WeaponPickup = Cast<AWeaponPickupActor>(OtherActor);
	if (WeaponPickup)
	{
		PickupWeapon = WeaponPickup;
	}
}

void ATestCharacter::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (OtherActor == PickupWeapon)
	{
		PickupWeapon = nullptr;
	}
}

void ATestCharacter::InvincibleActivate()
{
	//콜리전 끔
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	UE_LOG(LogTemp, Log, TEXT("Player Collision Disabled"));
}

void ATestCharacter::InvincibleDeactivate()
{
	//콜리전 켬
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	UE_LOG(LogTemp, Log, TEXT("Player Collision Enabled"));
}

void ATestCharacter::PlaySwordAttackMontage()
{
	PlayAnimMontage(SwordAttackMontage);
}

void ATestCharacter::PlaySwordRollMontage()
{
	PlayAnimMontage(SwordRollMontage);
}

void ATestCharacter::OnMovementInput(const FInputActionValue& InValue)
{
	FVector2D inputtedMovement = InValue.Get<FVector2D>();
	FVector moveDirection(inputtedMovement.Y, inputtedMovement.X, 0.0f);
	FQuat controlYawRotation = FQuat(FRotator(0, GetControlRotation().Yaw, 0));	// 컨트롤러의 Yaw회전을 따로 뽑아와서
	moveDirection = controlYawRotation.RotateVector(moveDirection);	// 이동 방향에 적용
	AddMovementInput(moveDirection);
}

void ATestCharacter::OnHorizonSightInput(const FInputActionValue& InValue)
{
	AddControllerYawInput((InValue.Get<float>()) * HorizonMouseSensivility);			//마우스 움직이는 값 * 마우스 감도만큼 움직임
}

void ATestCharacter::OnVerticalSightInput(const FInputActionValue& InValue)
{
	float sightMovingAmount = ((InValue.Get<float>()) * VerticalMouseSensivility) * Reverse;		//마우스 움직이는 값 * 마우스 감도만큼 움직임
	PlayerVerticalDegree += sightMovingAmount;
	if (PlayerVerticalDegree > MaxSightAngle)											//위로 넘어가지 않도록
	{
		PlayerVerticalDegree = MaxSightAngle;
		sightMovingAmount = 0.0f;
	}
	else if (PlayerVerticalDegree < -MaxSightAngle)										//아래로 넘어가지 않도록
	{
		PlayerVerticalDegree = -MaxSightAngle;
		sightMovingAmount = 0.0f;
	}
	
	AddControllerPitchInput(sightMovingAmount);
}

void ATestCharacter::OnAttackInput()
{
	UE_LOG(LogTemp, Log, TEXT("Attack Input Succed"));
	//WeaponManager->WeaponAttack(this);
	WeaponComponent->FireWeapon();
}

void ATestCharacter::OnInteract()
{
	if (PickupWeapon)
	{
		PickupWeapon->OnPickup(this);
		PickupWeapon = nullptr;
	}
}

void ATestCharacter::OnRollInput()
{
	UE_LOG(LogTemp, Log, TEXT("Roll Inputted"));
	if(AnimInstance.IsValid() && !AnimInstance->IsAnyMontagePlaying())
	{
		PlayerAnimation->PlayRollMontage();
	}
}

