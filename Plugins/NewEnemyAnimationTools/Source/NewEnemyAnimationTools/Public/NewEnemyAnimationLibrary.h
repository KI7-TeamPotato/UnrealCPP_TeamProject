#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NewEnemyAnimationLibrary.generated.h"

class UAnimBlueprint;
class UBlueprint;
class UAnimMontage;
class UBlueprint;
class USkeletalMesh;

/** Editor helper only. Generated ABPs do not reference this plugin. */
UCLASS()
class NEWENEMYANIMATIONTOOLS_API UNewEnemyAnimationLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Editor|NewEnemy")
    static bool ConfigureCharacterOwner(UAnimBlueprint* Blueprint, USkeletalMesh* PreviewMesh);

    // Replace only graph pin defaults in this Blueprint, without touching shared source assets.
    UFUNCTION(BlueprintCallable, Category = "Editor|NewEnemy")
    static int32 ReplaceGraphObjectReference(UBlueprint* Blueprint, UObject* Previous, UObject* Replacement);

    UFUNCTION(BlueprintCallable, Category = "Editor|NewEnemy")
    static bool SetMontageSlot(UAnimMontage* Montage, FName SlotName);
};
