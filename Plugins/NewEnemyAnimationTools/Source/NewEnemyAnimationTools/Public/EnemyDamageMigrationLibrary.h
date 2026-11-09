#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EnemyDamageMigrationLibrary.generated.h"

class UBlueprint;

/** Editor-only migration of source graphs; runtime assets reference only TeamPotato. */
UCLASS()
class NEWENEMYANIMATIONTOOLS_API UEnemyDamageMigrationLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Editor|Combat Migration")
    static bool IsBlueprintCompiled(UBlueprint* Blueprint);

    UFUNCTION(BlueprintCallable, Category = "Editor|Combat Migration")
    static int32 CountLegacyDamageCalls(UBlueprint* Blueprint);

    // Returns the number replaced, or -1 if validation fails. Does not compile or save assets.
    UFUNCTION(BlueprintCallable, Category = "Editor|Combat Migration")
    static int32 MigrateLegacyDamageCalls(UBlueprint* Blueprint);

    // Connects Self only to unspecified Owner pins on enemy bullet/pattern actor spawns.
    // Existing owner settings and spawns outside /Game/Enemy/Blueprint/Bullet are preserved.
    UFUNCTION(BlueprintCallable, Category = "Editor|Combat Migration")
    static int32 ConnectMissingSpawnOwners(UBlueprint* Blueprint);
};
