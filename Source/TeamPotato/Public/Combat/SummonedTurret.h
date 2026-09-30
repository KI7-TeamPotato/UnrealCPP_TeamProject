#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SummonedTurret.generated.h"

class AEnemyCharacter;
class ATurretProjectile;
class UBoxComponent;
class UStaticMeshComponent;
class UArrowComponent;

// Spawn from a GA with Owner/Instigator set to the summoning character.
UCLASS(Blueprintable)
class TEAMPOTATO_API ASummonedTurret : public AActor
{
    GENERATED_BODY()
public:
    ASummonedTurret();
    virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;

    // Used by the automatic timer. Respects FireInterval even when called from BP.
    UFUNCTION(BlueprintCallable, Category = "Turret")
    ATurretProjectile* FireAtNearestEnemy();
    UFUNCTION(BlueprintPure, Category = "Turret")
    AEnemyCharacter* FindNearestEnemy() const;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret|Components")
    TObjectPtr<UBoxComponent> Collision;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret|Components")
    TObjectPtr<UStaticMeshComponent> BaseMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret|Components")
    TObjectPtr<USceneComponent> AimPivot;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret|Components")
    TObjectPtr<UStaticMeshComponent> HeadMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret|Components")
    TObjectPtr<UArrowComponent> Muzzle;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turret|Attack")
    TSubclassOf<ATurretProjectile> ProjectileClass;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret|Attack", meta = (ClampMin = "1.0", Units = "cm"))
    float DetectionRange = 1200.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret|Attack", meta = (ClampMin = "0.05", Units = "s"))
    float FireInterval = 1.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret|Health", meta = (ClampMin = "1.0"))
    float MaxHealth = 100.0f;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Turret|Health")
    float CurrentHealth = 0.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret|Lifetime", meta = (ClampMin = "0.1", Units = "s"))
    float MaxLifetime = 60.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turret|Lifetime")
    bool bReplaceExistingTurret = true;

    UFUNCTION(BlueprintImplementableEvent, Category = "Turret", meta = (DisplayName = "On Projectile Fired"))
    void OnProjectileFired(ATurretProjectile* Projectile);
    UFUNCTION(BlueprintImplementableEvent, Category = "Turret", meta = (DisplayName = "On Turret Damaged"))
    void OnTurretDamaged(float ActualDamage);

protected:
    virtual void PostInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
    bool HasLineOfSight(const AEnemyCharacter* Enemy, const FVector& From) const;
    void AttackTimerTick();
    UFUNCTION()
    void OnSummonerDestroyed(AActor* DestroyedActor);
    TWeakObjectPtr<AActor> Summoner;
    FTimerHandle AttackTimer;
    double NextFireTime = 0.0;
};
