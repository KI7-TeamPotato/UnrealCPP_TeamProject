#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ProjectileSkillAbility.generated.h"

class ATurretProjectile;

// Configure a child GA on the character's SkillAbilityClass. The projectile owns damage and VFX.
UCLASS(Blueprintable)
class TEAMPOTATO_API UProjectileSkillAbility : public UGameplayAbility
{
    GENERATED_BODY()

public:
    UProjectileSkillAbility();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile Skill")
    TSubclassOf<ATurretProjectile> ProjectileClass;

    // Relative to the character's yaw: X forward, Y right, Z up. Units are centimeters.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile Skill", meta = (Units = "cm"))
    FVector SpawnOffset = FVector(100.0f, 0.0f, 30.0f);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile Skill", meta = (ClampMin = "1"))
    int32 BurstCount = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile Skill",
        meta = (EditCondition = "BurstCount > 1", ClampMin = "0.01", Units = "s"))
    float ShotInterval = 0.25f;

    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr,
        const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    // Optional cast cosmetics. No Event ActivateAbility graph is needed in the child GA.
    UFUNCTION(BlueprintImplementableEvent, Category = "Projectile Skill")
    void OnProjectileLaunched(ATurretProjectile* Projectile);

private:
    UFUNCTION()
    void FireNextShot();

    int32 ShotsFired = 0;
    bool GetLaunchTransform(const FGameplayAbilityActorInfo* ActorInfo, FTransform& OutTransform) const;
    static bool IsProjectileConfigured(const ATurretProjectile* Projectile);
};
