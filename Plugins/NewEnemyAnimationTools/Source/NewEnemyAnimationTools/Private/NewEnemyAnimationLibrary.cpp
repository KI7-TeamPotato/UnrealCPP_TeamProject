#include "NewEnemyAnimationLibrary.h"

#include "Animation/AnimBlueprint.h"
#include "Animation/AnimMontage.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/Character.h"
#include "K2Node_DynamicCast.h"
#include "Kismet2/BlueprintEditorUtils.h"

bool UNewEnemyAnimationLibrary::SetMontageSlot(UAnimMontage* Montage, FName SlotName)
{
    if (!Montage || Montage->SlotAnimTracks.Num() != 1 || SlotName.IsNone()) return false;
    Montage->Modify();
    Montage->SlotAnimTracks[0].SlotName = SlotName;
    return true;
}

int32 UNewEnemyAnimationLibrary::ReplaceGraphObjectReference(UBlueprint* Blueprint, UObject* Previous, UObject* Replacement)
{
    if (!Blueprint || !Previous || !Replacement || Previous == Replacement) return 0;
    TArray<UEdGraph*> Graphs;
    Blueprint->GetAllGraphs(Graphs);
    int32 Count = 0;
    for (UEdGraph* Graph : Graphs)
    {
        for (UEdGraphNode* Node : Graph->Nodes)
        {
            for (UEdGraphPin* Pin : Node->Pins)
            {
                if (Pin && Pin->DefaultObject == Previous)
                {
                    Node->Modify();
                    Pin->DefaultObject = Replacement;
                    ++Count;
                }
            }
        }
    }
    if (Count > 0) FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
    return Count;
}

bool UNewEnemyAnimationLibrary::ConfigureCharacterOwner(UAnimBlueprint* Blueprint, USkeletalMesh* PreviewMesh)
{
    if (!Blueprint || !PreviewMesh || PreviewMesh->GetSkeleton() != Blueprint->TargetSkeleton)
    {
        return false;
    }

    TArray<UK2Node_DynamicCast*> Casts;
    FBlueprintEditorUtils::GetAllNodesOfClass(Blueprint, Casts);
    if (Casts.Num() != 1)
    {
        return false;
    }

    UK2Node_DynamicCast* Cast = Casts[0];
    UEdGraphPin* Result = Cast->GetCastResultPin();
    if (!Result || Result->LinkedTo.IsEmpty())
    {
        return false;
    }

    Blueprint->Modify();
    Cast->Modify();
    // Preserve connections while reconstructing the old EliteEnemy cast.
    // Its consumers are already typed as Character in the reference graph.
    Result->PinType.PinSubCategoryObject = ACharacter::StaticClass();
    Result->PinName = FName(UEdGraphSchema_K2::PN_CastedValuePrefix + ACharacter::StaticClass()->GetName());
    Cast->TargetType = ACharacter::StaticClass();
    Cast->ReconstructNode();
    Result = Cast->GetCastResultPin();
    if (!Result || Result->LinkedTo.IsEmpty())
    {
        return false;
    }

    Blueprint->SetPreviewMesh(PreviewMesh);
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
    return true;
}
