#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/EnumBase.h"
#include "BulletBase.generated.h"

class UCapsuleComponent;
class UProjectileMovementComponent;
UCLASS()
class TEAMPOTATO_API ABulletBase : public AActor
{
	GENERATED_BODY()
	
public:
    ABulletBase();

protected:
    virtual void BeginPlay() override;

public:
    void Launch(const FVector& StartLocation, const FVector& Direction, float Speed);

    void Deactivate();

    bool IsActive() const { return bIsActive; }

protected:
    void OnLifeTimeExpired();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UCapsuleComponent> CapsuleComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> MeshComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    TObjectPtr<UProjectileMovementComponent> MovementComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet Settings", meta = (ExposeOnSpawn = "true"))
    EBulletType BulletType;

    UPROPERTY(EditAnywhere, Category = "Bullet Settings")
    float RollingStrength = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float ProjectileSpeed = 3000.0f; 
private:
    bool bIsActive;

    FTimerHandle LifeTimerHandle;

    const float MaxLifeTime = 3.0f;

};
