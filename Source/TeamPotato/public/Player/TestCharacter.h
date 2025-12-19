// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/EnumBase.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "TestCharacter.generated.h"

class AWeaponPickupActor;

UCLASS()
class TEAMPOTATO_API ATestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATestCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 입력과 이벤트 바인딩
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//무적 활성화
	void InvincibleActivate();
	//무적 비활성화
	void InvincibleDeactivate();

	//애니메이션만 재생하는 함수
	//검 상태 회피
	void PlayDodgeMontage_Front_Sword();
    void PlayDodgeMontage_Right_Sword();
    void PlayDodgeMontage_Left_Sword();
    void PlayDodgeMontage_Back_Sword();

    //총 상태 회피
    void PlayDodgeMontage_Front_Gun();
    void PlayDodgeMontage_Back_Gun();
    void PlayDodgeMontage_Right_Gun();
    void PlayDodgeMontage_Left_Gun();

	//검 공격 몽타주 재생
	void PlaySwordAttackMontage();

	//총 발사 몽타주 재생
	void PlayGunShootingMontage();
	
    //주울 아이템과 오버랩 됐을때 호출
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
    //주울 아이템과 오버랩이 끝났을때 호출
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

    //데미지 받는 델리게이트
    UFUNCTION(BlueprintCallable, Category = "Damage")
    void TakeAnyDamage(AActor* DamagedActor, float InDamage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

    //사망 처리
	UFUNCTION(BlueprintCallable, Category = "Kill")
	void KillPlayer();

    //대각선 방향 애니메이션 재생을 위해 몸 돌리기
    UFUNCTION()
    void RotatePlayer(EMovingDirection TurnDirection);

    //총 상태에서 회피 애니메이션 재생시 원활하게 움직이도록 AnimInstance의 RootMotionMode::IgnoreRootMotion 설정
    UFUNCTION()
    void SetAnimRootMotionIgnore();
    //RootMotionMode::RootMotionFromMontagesOnly으로 원상복구
    UFUNCTION()
    void SetAnimRootMotionFromMontage();

    //스태미너 사용 가능 여부 반환 및 스태미너 사용
    UFUNCTION()
    bool UseStamina(float InStaminaUseAmount);
    //스태미너 충전
    UFUNCTION(BlueprintCallable, Category = "Resource|Stamina")
    void FillStamina(float InStamina);

    //체력회복
    UFUNCTION(BlueprintCallable, Category = "Resource|Health")
    void Heal(float InHeal);
    
    //공격력 증가
    UFUNCTION(BlueprintCallable, Category = "Resource|Power")
    void AddPower(float InPower);
    //최대체력 증가
    UFUNCTION(BlueprintCallable, Category = "Resource|Health")
    void AddMaxHealth(float InMaxHealth);
    //최대 스태미나 증가
    UFUNCTION(BlueprintCallable, Category = "Resource|Stamina")
    void AddMaxStamina(float InMaxStamina);
    
    //Getter
	//현재 활성화된 무기 확인
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	inline EWeaponType GetPlayerActivatedWeapon() { return ActivatedWeapon; }

	UFUNCTION()
	inline UWeaponComponent* GetWeaponComponent() { return WeaponComponent; }

	UFUNCTION(BlueprintCallable, Category = "Sight")
	inline float GetSightDegree() { return SightDegree; }

    //마지막으로 입력받은 값을 enum으로 반환
    UFUNCTION()
    EMovingDirection GetLastInput();

    //Setter
    //행동중인지 설정
    UFUNCTION()
    void SetOnActing(bool InActing);

	//활성화 된 무기 설정
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	inline void SetPlayerActivatedWeapon(EWeaponType InActivatedWeapon) { ActivatedWeapon = InActivatedWeapon; }

protected:
	// 앞뒤양옆으로 움직이는 함수
	UFUNCTION()
	void OnMovementInput(const FInputActionValue& InValue);

    //앞뒤 움직임 입력
    UFUNCTION()
    void OnFrontMovementInput(const FInputActionValue& InValue);

    //앞뒤 움직임 입력 종료
    UFUNCTION()
    void OnFrontMovementComplete();

    //좌우 움직임 입력
    UFUNCTION()
    void OnSideMovementInput(const FInputActionValue& InValue);

    //좌우 움직임 입력 종료
    UFUNCTION()
    void OnSideMovementComplete();

	//고개 옆으로 돌리는 함수
	UFUNCTION()
	void OnHorizonSightInput(const FInputActionValue& InValue);

	//고개 위아래로 움직이는 함수
	UFUNCTION()
	void OnVerticalSightInput(const FInputActionValue& InValue);


	//구르기 함수
	UFUNCTION()
	void OnRollInput();

	//공격 함수
	UFUNCTION()
	void OnAttackInput();

    UFUNCTION()
    void OnSkillInput();

	// 상호작용 함수
	void OnInteract();

private:
    UFUNCTION()
    bool IsActionAvailable();

    UFUNCTION()
    void OnHitInvincible();

public:
	// 획득할 수 잇는 무기
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<AWeaponPickupActor> PickupWeapon = nullptr;

protected:
	//IA
	//이동 입력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction")
	TObjectPtr<UInputAction> IA_FrontMove = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction")
    TObjectPtr<UInputAction> IA_SideMove = nullptr;
	//횡방향 마우스 이동
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction")
	TObjectPtr<UInputAction> IA_HorizonSight = nullptr;
	//종방향 마우스 이동
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction")
	TObjectPtr<UInputAction> IA_VerticalSight = nullptr;
	//공격 입력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction")
	TObjectPtr<UInputAction> IA_Attack = nullptr;
	//구르기 입력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction")
	TObjectPtr<UInputAction> IA_Roll = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction")
    TObjectPtr<UInputAction> IA_Skill = nullptr;

	// 상호작용 입력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction")
	TObjectPtr<UInputAction> IA_Interact = nullptr;

	//components
	//스프링 암 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class USpringArmComponent> SpringArm = nullptr;
	//카메라 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class UCameraComponent> CameraComponent = nullptr;
	//움직임 컴포넌트
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class UCharacterMovementComponent> MovementComponent = nullptr;

	//스프링 암 길이
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component|Offset")
	float SpringArmLenght = 300.0f;
	//스프링 암 끄트머리(카메라) 위치조정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component|Offset")
	FVector SpringArmOffset = FVector(0, 0, 200.0f);
	//카메라 각도 조정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component|Offset")
	FQuat CameraRotation = FQuat(FRotator(-20.0f, 0, 0));

	//Anim Montage
	// 검
	//회피(구르기)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|Montage")
	TObjectPtr<class UAnimMontage> RollMontage_Sword = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|Montage")
	TObjectPtr<class UAnimMontage> BackStepMontage_Sword = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|Montage")
	TObjectPtr<class UAnimMontage> RightStepMontage_Sword = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|Montage")
	TObjectPtr<class UAnimMontage> LeftStepMontage_Sword = nullptr;
	//공격
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animaiton|Montage")
	TObjectPtr<class UAnimMontage> AttackMontage_Sword = nullptr;

	//총
    //회피(구르기)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animaiton|Montage")
	TObjectPtr<class UAnimMontage> RollMontage_Gun = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animaiton|Montage")
	TObjectPtr<class UAnimMontage> BackStepMontage_Gun = nullptr;
    //공격
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animaiton|Montage")
	TObjectPtr<class UAnimMontage> AttackMontage_Gun = nullptr;


	//무기 매니저
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<class AWeaponManagerActor> WeaponManager = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<class UWeaponComponent> WeaponComponent = nullptr;

private:
	//가로방향 마우스 감도
	float HorizonMouseSensivility = 1.0f;
	//세로방향 마우스 감도
	float VerticalMouseSensivility = 0.7f;

	//위아래 시야 움직임 제한치
	float MaxSightAngle = 20.0f;

	//마우스 상하 반전을 위함
	float Reverse = -1.0f;

	//플레이어 위아래 시야 각도
	float PlayerVerticalDegree = 0.0f;

	//플레이어가 행동중인지 확인
	bool bIsOnAction = false;

	//시야각
	float SightDegree = 0.0f;

    //피격무적 시간
    float OnHitInvincibleTime = 1.0f;

    bool bIsCanTakeDamage = true;
	
    //무기, 상황별로 구분해서 맞는 애니메이션 호출하는 함수
	UPROPERTY()
	TObjectPtr<class UPlayerAnimation> PlayerAnimation = nullptr;

	//ABP
	UPROPERTY()
	TWeakObjectPtr<class UAnimInstance> AnimInstance = nullptr;

    //리소스(체력, 마나 등)의 정보가 들어있음
	UPROPERTY()
	TObjectPtr<class UPlayerResource> ResourceManager = nullptr;

	//가지고 있는 무기 정보
    UPROPERTY()
	EWeaponType ActivatedWeapon = EWeaponType::None;

    //마지막 입력 저장하는 함수
    float FrontBackMove = 0.0f;
    float SideMove = 0.0f;

    //대각선 방향 애니메이션 재생을 위해 몸을 돌릴 각도
    float AnimRotateDegree = 45.0f;

    //회피 애니메이션 중 움직이지 않는 애니메이션 재생시 캐릭터를 움직일 정도
    float LaunchPlayerPower = 1000.0f;
};
