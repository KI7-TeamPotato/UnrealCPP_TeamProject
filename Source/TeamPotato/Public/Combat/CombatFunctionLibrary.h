#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Combat/CombatStatusDefinition.h"
#include "CombatFunctionLibrary.generated.h"

UCLASS()
class TEAMPOTATO_API UCombatFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Combat")
    static bool CanActorAttack(AActor* Actor);

    // Only player characters (TestCharacter subclasses) can receive these debuffs.
    UFUNCTION(BlueprintCallable, Category = "Combat|Status")
    static bool ApplyCombatStatus(AActor* Target, UCombatStatusDefinition* Definition, AActor* DamageCauser, AController* InstigatorController = nullptr);

    // Normal hit damage still works on enemies; only the status payload is player-only.
    UFUNCTION(BlueprintCallable, Category = "Combat|Status")
    static float ApplyCombatDamageWithStatuses(AActor* Target, float Damage, AActor* DamageCauser,
        const TArray<UCombatStatusDefinition*>& Statuses, AController* InstigatorController = nullptr);

    static float ApplyCombatHitWithStatuses(AActor* Target, float Damage, AActor* DamageCauser,
        const TArray<UCombatStatusDefinition*>& Statuses, AController* InstigatorController,
        const FHitResult* Hit, TSubclassOf<UDamageType> DamageType = nullptr);

    // Returns actual HP removed, not the requested (possibly overkill) damage.
    UFUNCTION(BlueprintCallable, Category = "Combat", meta = (DefaultToSelf = "DamageCauser"))
    static float ApplyCombatDamage(AActor* Target, float Damage, AActor* DamageCauser, AController* InstigatorController = nullptr,
        TSubclassOf<UDamageType> DamageType = nullptr);

    static float ApplyCombatDamageWithHit(AActor* Target, float Damage, AActor* DamageCauser,
        AController* InstigatorController, const FHitResult* Hit, TSubclassOf<UDamageType> DamageType = nullptr);
};
