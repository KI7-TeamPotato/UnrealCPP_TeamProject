// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TestCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Item/Weapon/WeaponManagerActor.h"
#include "Component/WeaponComponent.h"
#include "Item/Weapon/WeaponPickupActor.h"
#include "GameFramework/SpringArmComponent.h"
#include "Component/PlayerResource.h"
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

	ActivatedWeapon = EWeaponType::Gun;			//테스트용

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));

	ResourceManager = CreateDefaultSubobject<UPlayerResource>(TEXT("ResourceManager"));
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

void ATestCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
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

void ATestCharacter::KillPlayer()
{
	// 1. 캐릭터 이동 중지
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();

	// 2. 캡슐 충돌 끄기 (중요!!)
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 3. 메시를 Ragdoll 프로파일로
	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));

	// 4. 물리 활성화
	MeshComp->SetSimulatePhysics(true);

	// 5. 캡슐에서 분리 (선택이지만 거의 필수)
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
	PlayAnimMontage(SwordAttackMontage);
}

void ATestCharacter::PlayGunShootingMontage()
{
	PlayAnimMontage(GunShootMontage);
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
		WeaponComponent->FireWeapon();
	}
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
	//KillPlayer();				//테스트용으로 사망 연출 실행해봄
	if (!bIsOnActing)
	{
		if (ResourceManager->UseStamina(RollStamina))
		{
			PlayerAnimation->PlayRollMontage();
		}
	}
}

