#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CombatFunctionLibrary.generated.h"

UCLASS()
class TEAMPOTATO_API UCombatFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // Returns actual HP removed, not the requested (possibly overkill) damage.
    UFUNCTION(BlueprintCallable, Category = "Combat", meta = (DefaultToSelf = "DamageCauser"))
    static float ApplyCombatDamage(AActor* Target, float Damage, AActor* DamageCauser, AController* InstigatorController = nullptr,
        TSubclassOf<UDamageType> DamageType = nullptr);

    static float ApplyCombatDamageWithHit(AActor* Target, float Damage, AActor* DamageCauser,
        AController* InstigatorController, const FHitResult* Hit, TSubclassOf<UDamageType> DamageType = nullptr);
};
