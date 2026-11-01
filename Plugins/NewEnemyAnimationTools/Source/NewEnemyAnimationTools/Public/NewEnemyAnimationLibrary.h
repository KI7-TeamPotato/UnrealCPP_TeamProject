#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NewEnemyAnimationLibrary.generated.h"

class UAnimBlueprint;
class UBlueprint;
class UAnimMontage;
class USkeletalMesh;
class UBehaviorTree;
class UBTTaskNode;
class UBlackboardData;

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

    // Replace the CombatState branch in both the runtime tree and its editable graph.
    UFUNCTION(BlueprintCallable, Category = "Editor|NewEnemy")
    static bool ReplaceRangedCombatBranch(UBehaviorTree* Tree, TSubclassOf<UBTTaskNode> TaskClass, UBlackboardData* Blackboard);
};
