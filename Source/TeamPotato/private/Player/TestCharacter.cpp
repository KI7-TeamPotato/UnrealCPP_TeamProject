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

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));

	ResourceManager = CreateDefaultSubobject<UPlayerResource>(TEXT("ResourceManager"));

    //데미지 받는 함수 바인딩
    OnTakeAnyDamage.AddDynamic(this, &ATestCharacter::TakeAnyDamage);
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
		enhancedInputComponent->BindAction(IA_FrontMove, ETriggerEvent::Triggered, this, &ATestCharacter::OnFrontMovementInput);
		enhancedInputComponent->BindAction(IA_FrontMove, ETriggerEvent::Completed, this, &ATestCharacter::OnFrontMovementComplete); 
		enhancedInputComponent->BindAction(IA_SideMove, ETriggerEvent::Triggered, this, &ATestCharacter::OnSideMovementInput);
		enhancedInputComponent->BindAction(IA_SideMove, ETriggerEvent::Completed, this, &ATestCharacter::OnSideMovementComplete);
		enhancedInputComponent->BindAction(IA_HorizonSight, ETriggerEvent::Triggered, this, &ATestCharacter::OnHorizonSightInput);
		enhancedInputComponent->BindAction(IA_VerticalSight, ETriggerEvent::Triggered, this, &ATestCharacter::OnVerticalSightInput);
		enhancedInputComponent->BindAction(IA_Attack, ETriggerEvent::Started, this, &ATestCharacter::OnAttackInput);
		enhancedInputComponent->BindAction(IA_Roll, ETriggerEvent::Started, this, &ATestCharacter::OnRollInput);
		enhancedInputComponent->BindAction(IA_Interact, ETriggerEvent::Started, this, &ATestCharacter::OnInteract);
		enhancedInputComponent->BindAction(IA_Skill, ETriggerEvent::Started, this, &ATestCharacter::OnSkillInput);
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

void ATestCharacter::TakeAnyDamage(AActor* DamagedActor, float InDamage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
    if(bIsCanTakeDamage)
    {
        ResourceManager->PlayerTakeDamage(InDamage);
        OnHitInvincible();
    }
}


void ATestCharacter::KillPlayer()
{
	//캐릭터 이동 중지
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();

	//캡슐 충돌 끄기 
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//메시를 Ragdoll 프로파일로
	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));

	//물리 활성화
	MeshComp->SetSimulatePhysics(true);

	//캡슐에서 분리 
	MeshComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

    //애니메이션 재생을 막기 위함
    bIsOnAction = true;
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
    if(!bIsOnAction)
    {
        FrontBackMove = InValue.Get<float>();
        if (Controller && (FrontBackMove != 0.0f))
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
    if (!bIsOnAction)
    {
        SideMove = InValue.Get<float>();
        if (Controller && (SideMove != 0.0f))
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
	AddControllerYawInput((InValue.Get<float>()) * HorizonMouseSensivility);	        //마우스 움직이는 값 * 마우스 감도만큼 움직임
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
	//if(!bIsOnAction)
	//{
 //       if(UseStamina(10))            //테스트용. 스태미너 사용은 무기에서 할 예정.
 //       {
 //           PlayerAnimation->PlayAttackAnimation();     
 //           WeaponComponent->WeaponAttack();
 //       }
	//}

    if (IsActionAvailable())
    {
        PlayerAnimation->PlayAttackAnimation();
        WeaponComponent->WeaponAttack();
    }
}

void ATestCharacter::OnSkillInput()
{
    UE_LOG(LogTemp, Log, TEXT("Player Skill Active"));
}

void ATestCharacter::OnInteract()
{
	if (PickupWeapon)
	{
		PickupWeapon->OnPickup(this);
		PickupWeapon = nullptr;
	}
}

bool ATestCharacter::IsActionAvailable()
{
    if (!bIsOnAction && (ActivatedWeapon != EWeaponType::None))
    {
        if(UseStamina(10))                      //테스트용
            return true;
    }
    return false;
}

void ATestCharacter::OnHitInvincible()
{
    InvincibleActivate();
    bIsCanTakeDamage = false;

    FTimerHandle timerHandle;
    GetWorldTimerManager().SetTimer(
        timerHandle,
        FTimerDelegate::CreateLambda([this]()
            {
                bIsCanTakeDamage = true;
                InvincibleDeactivate();
            }),
        OnHitInvincibleTime,
        false
    );
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
    bIsOnAction = true;
}

void ATestCharacter::SetAnimRootMotionIgnore()
{
    AnimInstance->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
}

void ATestCharacter::SetAnimRootMotionFromMontage()
{
    AnimInstance->SetRootMotionMode(ERootMotionMode::RootMotionFromMontagesOnly);
}

bool ATestCharacter::UseStamina(float InStaminaUseAmount)
{
    return ResourceManager->UseStamina(InStaminaUseAmount);
}

void ATestCharacter::FillStamina(float InStamina)
{
    ResourceManager->FillStamina(InStamina);
}

void ATestCharacter::Heal(float InHeal)
{
    ResourceManager->Heal(InHeal);
}

void ATestCharacter::AddPower(float InPower)
{
    ResourceManager->AddPower(InPower);
}

void ATestCharacter::AddMaxHealth(float InMaxHealth)
{
    ResourceManager->AddMaxHealth(InMaxHealth);
}

void ATestCharacter::AddMaxStamina(float InMaxStamina)
{
    ResourceManager->AddMaxStamina(InMaxStamina);
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
    if(!InActing)
    {
        MovementComponent->bUseControllerDesiredRotation = true;
        MovementComponent->bOrientRotationToMovement = false;
        bUseControllerRotationYaw = true;

        if(Controller)
        {
            FRotator controllerRotation = Controller->GetControlRotation();
            SetActorRotation(controllerRotation);
        }
    }
    bIsOnAction = InActing;
}

void ATestCharacter::OnRollInput()
{
	//KillPlayer();				//테스트용으로 사망 연출 실행해봄
	if (IsActionAvailable())
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

