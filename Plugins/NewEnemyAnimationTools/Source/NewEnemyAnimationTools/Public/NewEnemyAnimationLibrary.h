#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NewEnemyAnimationLibrary.generated.h"

class UAnimBlueprint;
class UBlueprint;
class UAnimMontage;
class UAnimSequence;
class UAnimSequenceBase;
class USkeletalMesh;
class USkeletalMeshComponent;
class UBehaviorTree;
class UBTTaskNode;
class UBlackboardData;
class UMaterial;
class UMaterialExpression;

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

    // Replaces the slots and sections with one full-length segment in a registered slot.
    // Clears existing montage notifies for the caller to recreate. Preserves blend settings and does not save.
    UFUNCTION(BlueprintCallable, Category = "Editor|NewEnemy")
    static bool ConfigureSingleSequenceMontage(UAnimMontage* Montage, UAnimSequence* Sequence, FName SlotName);

    // Evaluates a paused SingleNode pose immediately for an editor preview component.
    UFUNCTION(BlueprintCallable, Category = "Editor|NewEnemy")
    static bool EvaluateSingleSequencePose(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Sequence, float Time);

    // Replace the CombatState branch in both the runtime tree and its editable graph.
    UFUNCTION(BlueprintCallable, Category = "Editor|NewEnemy")
    static bool ReplaceRangedCombatBranch(UBehaviorTree* Tree, TSubclassOf<UBTTaskNode> TaskClass, UBlackboardData* Blackboard);

    // MP_MaterialAttributes is hidden from Python's enum, so expose its existing connection explicitly.
    UFUNCTION(BlueprintCallable, Category = "Editor|NewEnemy")
    static UMaterialExpression* GetMaterialAttributesInput(UMaterial* Material, FString& OutputName);

    UFUNCTION(BlueprintCallable, Category = "Editor|NewEnemy")
    static bool ConnectMaterialAttributes(UMaterial* Material, UMaterialExpression* Expression, const FString& OutputName);

    UFUNCTION(BlueprintCallable, Category = "Editor|NewEnemy")
    static UMaterialExpression* CreateEmissiveAttributeOverride(UMaterial* Material, int32 X, int32 Y);

    UFUNCTION(BlueprintCallable, Category = "Editor|NewEnemy")
    static FString GetSkeletalMeshSectionSummary(USkeletalMesh* SkeletalMesh);

    // Returns changed section count, zero when already disabled, or -1 on validation failure. Does not save.
    UFUNCTION(BlueprintCallable, Category = "Editor|NewEnemy")
    static int32 DisableSkeletalMeshMaterialSections(USkeletalMesh* SkeletalMesh, const TArray<int32>& MaterialSlots);
};
