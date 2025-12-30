#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyCharacter.h"
#include "Component/BulletHellComponent.h"
#include "BossBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPatternFinished);
// 보스가 스폰가 사망 시에 호출되는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossSpawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDie);
// 보스의 체력이 바뀌면 호출되는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossHealthChanged, float, CurrentHealth, float, MaxHealth);

UCLASS()
class TEAMPOTATO_API ABossBase : public AEnemyCharacter
{
    GENERATED_BODY()

public:
    ABossBase();

protected:
    virtual void BeginPlay() override;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    virtual void OnDie() override;

    virtual float SetMovementSpeed_Implementation(EEnemySpeed State) override;

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Status")
    int32 CurrentPhase = 1;

    UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
    virtual void ExecutePattern(int32 PatternIndex);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
    UBulletHellComponent* BulletPatternComp;

    UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
    void FinishCurrentPattern();

    UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
    FOnPatternFinished OnPatternFinished;

    // 보스가 스폰되었을 때 호출되는 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
    FOnBossSpawn OnBossSpawn;

    // 보스가 사망했을 때 호출되는 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
    FOnBossDie OnBossDie;

    UPROPERTY(BlueprintAssignable, Category = "Boss|Event")
    FOnBossHealthChanged OnBossHealthChanged;

protected:
    UFUNCTION(BlueprintNativeEvent, Category = "Boss|Combat")
    void PatternOne();

    UFUNCTION(BlueprintNativeEvent, Category = "Boss|Combat")
    void PatternTwo();

    UFUNCTION(BlueprintNativeEvent, Category = "Boss|Combat")
    void PatternThree();

    UFUNCTION(BlueprintNativeEvent, Category = "Boss|Combat")
    void PatternFour();

    UFUNCTION(BlueprintNativeEvent, Category = "Boss|Combat")
    void PatternFive();
};