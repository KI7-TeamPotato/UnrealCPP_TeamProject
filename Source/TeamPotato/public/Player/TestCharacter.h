// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "Weapon/WeaponBase.h"
#include "TestCharacter.generated.h"

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
	//구르기
	void PlaySwordRollMontage();

	//공격
	void PlaySwordAttackMontage();

	inline EWeaponType GetPlayerActivatedWeapon() { return ActivatedWeapon; }

protected:
	// 앞뒤양옆으로 움직이는 함수
	UFUNCTION()
	void OnMovementInput(const FInputActionValue& InValue);

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
private:

public:
	//가지고 있는 무기 정보
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeaponType")
	EWeaponType ActivatedWeapon = EWeaponType::None;

protected:
	//IA
	//이동 입력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputAction")
	TObjectPtr<UInputAction> IA_Move = nullptr;
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
	//구르기 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation|Montage")
	TObjectPtr<class UAnimMontage> SwordRollMontage = nullptr;
	//공격 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animaiton|Montage")
	TObjectPtr<class UAnimMontage> SwordAttackMontage = nullptr;

	//무기 매니저
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<class AWeaponManagerActor> WeaponManager = nullptr;

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


	
	UPROPERTY()
	TObjectPtr<class UPlayerAnimation> PlayerAnimation = nullptr;

	//ABP
	UPROPERTY()
	TWeakObjectPtr<class UAnimInstance> AnimInstance = nullptr;

};
