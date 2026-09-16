

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Data/EnumBase.h"
#include "Intetface/EnemyInterface.h"
#include "AbilitySystemInterface.h"
#include "EnemyCharacter.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDying);

class UWidgetComponent;
class UEnemyHealthBarWidget;
class UPoolingSubsystem;
class UCombatAbilitySystemComponent;
struct FGameplayEffectContextHandle;

UCLASS()
class TEAMPOTATO_API AEnemyCharacter : public ACharacter, public IEnemyInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    UFUNCTION(BlueprintGetter, Category = "Combat")
    float GetCombatHealth() const;
    UFUNCTION(BlueprintGetter, Category = "Combat")
    float GetCombatMaxHealth() const;
    UFUNCTION(BlueprintSetter, Category = "Combat")
    void SetCombatMaxHealth(float NewMaxHealth);

    //몬스터 사망 시 실행되는 함수
    UFUNCTION(BlueprintCallable)
    virtual void OnDie();

    //이 몬스터의 분류(보스, 엘리트, 일반)을 결정하는 함수
    UPROPERTY(VisibleAnywhere)
    bool ImNormal = true;

    UPROPERTY(VisibleAnywhere)
    float Elitemultiple = 1.0f;

    UPROPERTY(VisibleAnywhere)
    float Bossmultiple = 1.0f;

    //적을 처치시 나올 아이템들의 클래스
    UPROPERTY(VisibleAnywhere, Category = "Drops")
    TSubclassOf<class APickupHealthActor> HealthPickupClass;

    UPROPERTY(VisibleAnywhere, Category = "Drops")
    TSubclassOf<class APickupStaminaActor> StaminaPickupClass;

    UPROPERTY(VisibleAnywhere, Category = "Drops")
    TSubclassOf<class APickupGoldActor> GoldPickupClass;

    //블루프린트에서 어떤 아이템이 나올지 설정할 수 있게 만든 함수setter
    UFUNCTION(BlueprintCallable, Category = "Drops")
    void SetDropItemClasses(
        TSubclassOf<class APickupHealthActor> InHealthClass,
        TSubclassOf<class APickupStaminaActor> InStaminaClass,
        TSubclassOf<class APickupGoldActor> InGoldClass
    );
protected:
    virtual void PostInitializeComponents() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void HandleCombatHealthChanged(float NewHealth, float NewMaxHealth);
    void HandleCombatDamage(float ActualDamage, const FGameplayEffectContextHandle& Context);
    bool bDeathHandled = false;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    //데미지를 입을때 호출하는 함수
    UFUNCTION()
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;

public:	
    //착용할 무기 블루프린트에서 할당함
    UPROPERTY(EditAnywhere, Category = "Combat")
    TSubclassOf<AActor> WeaponClass;

    // 현재 무기
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
        AActor* CurrentWeapon;

    //공격 애니메이션
    UPROPERTY(EditAnywhere, Category = "Combat")
    UAnimMontage* AttackMontage;

    //사망 애니메이션
    UPROPERTY(EditAnywhere, Category = "Combat")
    UAnimMontage* DeadMontage;

    //소켓 이름(나중에 통일할 예정)
    UPROPERTY(EditAnywhere, Category = "Combat")
    FName WeaponSocketName = TEXT("RHandSoket");

    //상태 확인용 변수 (BT 데코레이터용)
    UPROPERTY(BlueprintReadWrite, Category = "Combat")
    bool bIsWieldingWeapon;

    //무기 장착 함수 나중에 WieldWeapon으로 수정해야 함
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void WieldWeapon();

    //공격용 함수
    UFUNCTION(BlueprintCallable, Category = "Combat")
    virtual void DefaultAttack();

    //플레이어 방향으로 방향 전환하는 함수
    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool PlayerFocus(AActor* TargetActor, float DeltaTime, float TurnSpeed);

    //이동속도 설정
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void UpdateMovementSpeed(float NewSpeed);

    //인터페이스 함수 오버라이드, EnumBase에 있는 열거형에 따라 상태별 속도 설정
    virtual float SetMovementSpeed_Implementation(EEnemySpeed State) override;

    //죽었을때 나오는 델리게이트
    UPROPERTY(BlueprintAssignable)
    FOnEnemyDying OnDeath;

private:
    // --- 체력 위젯 컴포넌트를 업데이트하는 함수 ---
    void SetupHealthBarWidget();

    // --- 체력 위젯 컴포넌트를 플레이어 방향으로 회전시키는 함수 ---
    void RotateHealthBarToViewport();

    //사망시 아이템 드롭하는 함수
    void EnemyItemDrop();

    //몇개 드롭할지(아이템마다 달라서 멤버변수로 설정함)
    int32 DropCount = 0;
protected:
    //체력 바 위젯
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UWidgetComponent> HealthBarWidgetComponent;

    //몬스터(일반 몬스터) 체력 기본값 35
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USceneComponent> DamagePopupSpawnPoint;

    UPROPERTY(EditAnywhere, BlueprintGetter = GetCombatMaxHealth, BlueprintSetter = SetCombatMaxHealth, Category = "Stats")
    float MaxHealth = 35.0f;

    //현재 체력
    // Kept for existing Blueprint pins; getter reads GAS and the field is a display mirror only.
    UPROPERTY(VisibleAnywhere, BlueprintGetter = GetCombatHealth, Category = "Stats")
    float CurrentHealth = 0.0f;
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<UCombatAbilitySystemComponent> CombatAbilitySystem;

    //몬스터 무적 시간
    UPROPERTY(EditAnywhere, Category = "Combat")
    float InvincibilityDuration = 0.3f;

private:

    UPROPERTY()
    TObjectPtr<UEnemyHealthBarWidget> HealthBarWidget;

    UPROPERTY()
    TObjectPtr<UPoolingSubsystem> PoolingSubsystem;
};
