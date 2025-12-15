// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/TestCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "Weapon/WeaponManagerActor.h"
#include "GameFramework/SpringArmComponent.h"

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

	if (bIsRolling)
	{
		// 애니메이션이 Actor 회전을 덮어도 매 프레임 입력 방향 유지
		//SetActorRotation(RollDir.Rotation());
	}
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
	}
}

void ATestCharacter::PlaySwordAttackMontage()
{
	PlayAnimMontage(SwordAttackMontage);
}

void ATestCharacter::OnMovementInput(const FInputActionValue& InValue)
{
	MoveInput = InValue.Get<FVector2D>();
	if (!bIsOnAction)
	{
		/*FRotator Rotation = Controller->GetControlRotation();
		FRotator YawRot(0, Rotation.Yaw, 0);

		FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

		AddMovementInput(Forward, MoveInput.Y);
		AddMovementInput(Right, MoveInput.X);*/

		FVector moveDirection(MoveInput.Y, MoveInput.X, 0.0f);

		//FQuat controlYawRotation = FQuat(FRotator(0, GetControlRotation().Yaw, 0));	// 컨트롤러의 Yaw회전을 따로 뽑아와서
		//moveDirection = controlYawRotation.RotateVector(moveDirection);	// 이동 방향에 적용

		AddMovementInput(moveDirection);
		/*FVector moveDirection(MoveInput.Y, MoveInput.X, 0.0f);
		AddMovementInput(moveDirection);*/
	}
}

void ATestCharacter::OnHorizonSightInput(const FInputActionValue& InValue)
{
	AddControllerYawInput((InValue.Get<float>()) * HorizonMouseSensivility);			//마우스 움직이는 값 * 마우스 감도만큼 움직임
}

void ATestCharacter::OnVerticalSightInput(const FInputActionValue& InValue)
{
	float sightMovingAmount = (InValue.Get<float>()) * VerticalMouseSensivility;		//마우스 움직이는 값 * 마우스 감도만큼 움직임
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
	WeaponManager->WeaponAttack(this);
}

void ATestCharacter::OnRollInput()
{
	if (!bIsOnAction)
	{
		//콜리전 끄기(무적)
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

		PlayRollMontage();

		// EndRoll은 몽타주 Notify 또는 Delay로 호출
		FTimerHandle TimerHandle;
		float onRolling = (RollMontage->GetPlayLength()) / 2;

		//구르는동안 무적
		GetWorldTimerManager().SetTimer(TimerHandle, [this] {}, onRolling, false);
		//콜리전 켜기(무적 종료)
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		//구르기 후딜레이동안 무적아님
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ATestCharacter::EndRollMontage, onRolling, false);
	}
}

void ATestCharacter::PlayRollMontage()
{
	if (AnimInstance.IsValid() && !GetController()->IsMoveInputIgnored())
	{
		// 1) 입력값 가져오기
		FVector2D Input = MoveInput;
		if (Input.IsNearlyZero())
		{
			// 입력이 없으면 캐릭터 정면으로 구르기
			Input.Y = 1.0f;
		}

		// 2) 입력을 월드 방향으로 변환
		FRotator CamRot = Controller->GetControlRotation();
		CamRot.Pitch = 0;
		CamRot.Roll = 0;

		FVector Forward = FRotationMatrix(CamRot).GetUnitAxis(EAxis::X);
		FVector Right = FRotationMatrix(CamRot).GetUnitAxis(EAxis::Y);

		RollDir = (Forward * Input.Y + Right * Input.X).GetSafeNormal();

		if (RollDir.IsNearlyZero())
			RollDir = Forward;

		// 3) 캐릭터를 입력 방향으로 회전
		SetActorRotation(RollDir.Rotation());

		// 4) 이동 처리
		//LaunchCharacter(RollDir * RollStrength, true, true);

		// 5) 몽타주 재생
		AnimInstance->Montage_Play(RollMontage);

		// 6) 상태 플래그
		bIsRolling = true;
		bIsOnAction = true;
	}
	
}

void ATestCharacter::EndRollMontage()
{
	bIsRolling = false;
	bIsOnAction = false;
}
