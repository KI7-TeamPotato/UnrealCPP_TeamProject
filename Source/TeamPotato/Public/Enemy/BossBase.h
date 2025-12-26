#pragma once

#include "CoreMinimal.h"
#include "Enemy/EnemyCharacter.h"
#include "Component/BulletHellComponent.h"
#include "BossBase.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPatternFinished);

UCLASS()
class TEAMPOTATO_API ABossBase : public AEnemyCharacter
{
    GENERATED_BODY()

public:
    ABossBase();

protected:
    virtual void BeginPlay() override;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

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