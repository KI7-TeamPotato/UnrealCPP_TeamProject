#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CombatStatusDefinition.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class ECombatStatus : uint8
{
    Poison,
    Fire,
    Electric
};

// Player-only debuff, applied by enemy attacks or hazards. GEs own duration/period/slow strength.
UCLASS(BlueprintType)
class TEAMPOTATO_API UCombatStatusDefinition : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
    ECombatStatus Status = ECombatStatus::Poison;

    // HasDuration, Period > 0, AggregateByTarget, limit 1, refresh duration, NeverReset period.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
    TSubclassOf<UGameplayEffect> DamageEffect;

    // Infinite, non-periodic GE modifying MoveSpeedMultiplier. Removed with the poison DOT.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status", meta = (EditCondition = "Status == ECombatStatus::Poison"))
    TSubclassOf<UGameplayEffect> PoisonSlowEffect;

    // Passed as Data.Damage. A GE can instead author a fixed Scalable Float magnitude.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status", meta = (ClampMin = "0.0"))
    float DamagePerTick = 5.0f;
};
