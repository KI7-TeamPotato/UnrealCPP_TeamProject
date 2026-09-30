#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UNiagaraComponent;
class UNiagaraSystem;

// Damage belongs to the projectile BP, not to the turret or the summoning ability.
UCLASS(Blueprintable)
class TEAMPOTATO_API ATurretProjectile : public AActor
{
    GENERATED_BODY()
public:
    ATurretProjectile();
    UFUNCTION(BlueprintCallable, Category = "Turret Projectile")
    bool Launch(FVector Direction);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Components")
    TObjectPtr<USphereComponent> Collision;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Components")
    TObjectPtr<UStaticMeshComponent> Mesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Components")
    TObjectPtr<UProjectileMovementComponent> Movement;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile|Components")
    TObjectPtr<UNiagaraComponent> Trail;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile|Effects")
    TObjectPtr<UNiagaraSystem> ImpactEffect;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (ClampMin = "0.0"))
    float Damage = 10.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (ClampMin = "1.0", Units = "cm/s"))
    float Speed = 1800.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile", meta = (ClampMin = "0.1", Units = "s"))
    float MaxLifetime = 5.0f;
private:
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
        FVector NormalImpulse, const FHitResult& Hit);
    UFUNCTION()
    void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
        int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    void ResolveImpact(AActor* OtherActor, const FHitResult& Hit);
    bool bLaunched = false;
    bool bImpacted = false;
};
