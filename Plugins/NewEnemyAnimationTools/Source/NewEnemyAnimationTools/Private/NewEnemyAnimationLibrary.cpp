#include "NewEnemyAnimationLibrary.h"

#include "Animation/AnimBlueprint.h"
#include "Animation/AnimMontage.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTreeGraph.h"
#include "BehaviorTreeGraphNode.h"
#include "BehaviorTreeGraphNode_Root.h"
#include "BehaviorTreeGraphNode_Task.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/Character.h"
#include "K2Node_DynamicCast.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "UObject/UnrealType.h"
#include "MaterialEditingLibrary.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionSetMaterialAttributes.h"
#include "Materials/MaterialAttributeDefinitionMap.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Rendering/SkeletalMeshLODRenderData.h"
#include "SkeletalMeshTypes.h"

namespace
{
    int32 EffectiveSectionMaterial(const USkeletalMesh& Mesh, const FSkeletalMeshLODInfo& Info,
        const FSkelMeshSection& Section, int32 SectionIndex)
    {
        const int32 Material = Info.LODMaterialMap.IsValidIndex(SectionIndex) && Info.LODMaterialMap[SectionIndex] != INDEX_NONE
            ? Info.LODMaterialMap[SectionIndex] : Section.MaterialIndex;
        return Mesh.GetMaterials().IsValidIndex(Material) ? Material : INDEX_NONE;
    }
}

FString UNewEnemyAnimationLibrary::GetSkeletalMeshSectionSummary(USkeletalMesh* SkeletalMesh)
{
    if (!SkeletalMesh || SkeletalMesh->IsCompiling()) return TEXT("error=invalid_or_compiling_mesh\n");
    const FSkeletalMeshModel* Model = SkeletalMesh->GetImportedModel();
    if (!Model) return TEXT("error=missing_imported_model\n");
    const FSkeletalMeshRenderData* Render = SkeletalMesh->GetResourceForRendering();
    FString Summary;
    for (int32 LOD = 0; LOD < Model->LODModels.Num(); ++LOD)
    {
        const FSkeletalMeshLODModel& LodModel = Model->LODModels[LOD];
        const FSkeletalMeshLODInfo* Info = SkeletalMesh->GetLODInfo(LOD);
        for (int32 Index = 0; Index < LodModel.Sections.Num(); ++Index)
        {
            const FSkelMeshSection& Section = LodModel.Sections[Index];
            const FSkelMeshSourceSectionUserData* User = LodModel.UserSectionsData.Find(Section.OriginalDataSectionIndex);
            const int32 RenderDisabled = Render && Render->LODRenderData.IsValidIndex(LOD) &&
                Render->LODRenderData[LOD].RenderSections.IsValidIndex(Index)
                ? int32(Render->LODRenderData[LOD].RenderSections[Index].bDisabled) : -1;
            Summary += FString::Printf(TEXT("lod=%d section=%d material=%d effective_material=%d disabled=%d source=%d source_disabled=%d chunk_parent=%d render_disabled=%d vertices=%d triangles=%d\n"),
                LOD, Index, int32(Section.MaterialIndex), Info ? EffectiveSectionMaterial(*SkeletalMesh, *Info, Section, Index) : -1,
                int32(Section.bDisabled), Section.OriginalDataSectionIndex, User ? int32(User->bDisabled) : -1,
                Section.ChunkedParentSectionIndex, RenderDisabled, Section.NumVertices, Section.NumTriangles);
        }
    }
    return Summary;
}

int32 UNewEnemyAnimationLibrary::DisableSkeletalMeshMaterialSections(USkeletalMesh* SkeletalMesh, const TArray<int32>& MaterialSlots)
{
    if (!SkeletalMesh || SkeletalMesh->IsCompiling()) return -1;
    FSkeletalMeshModel* Model = SkeletalMesh->GetImportedModel();
    if (!Model || Model->LODModels.Num() != SkeletalMesh->GetLODNum()) return -1;
    for (int32 Slot : MaterialSlots)
        if (!SkeletalMesh->GetMaterials().IsValidIndex(Slot)) return -1;
    if (MaterialSlots.IsEmpty()) return 0;

    TArray<FIntPoint> Changes;
    for (int32 LOD = 0; LOD < Model->LODModels.Num(); ++LOD)
    {
        const FSkeletalMeshLODInfo* Info = SkeletalMesh->GetLODInfo(LOD);
        if (!Info) return -1;
        const FSkeletalMeshLODModel& LodModel = Model->LODModels[LOD];
        TSet<int32> Roots;
        TSet<int32> Sources;
        for (int32 Index = 0; Index < LodModel.Sections.Num(); ++Index)
        {
            const FSkelMeshSection& Section = LodModel.Sections[Index];
            const int32 Material = EffectiveSectionMaterial(*SkeletalMesh, *Info, Section, Index);
            if (Material == INDEX_NONE) return -1;
            if (!MaterialSlots.Contains(Material)) continue;
            const int32 Root = Section.ChunkedParentSectionIndex == INDEX_NONE ? Index : Section.ChunkedParentSectionIndex;
            if (!LodModel.Sections.IsValidIndex(Root) || LodModel.Sections[Root].ChunkedParentSectionIndex != INDEX_NONE) return -1;
            Roots.Add(Root);
            Sources.Add(Section.OriginalDataSectionIndex);
            Sources.Add(LodModel.Sections[Root].OriginalDataSectionIndex);
        }
        for (int32 Index = 0; Index < LodModel.Sections.Num(); ++Index)
        {
            const FSkelMeshSection& Section = LodModel.Sections[Index];
            if (!Roots.Contains(Index) && !Roots.Contains(Section.ChunkedParentSectionIndex) &&
                !Sources.Contains(Section.OriginalDataSectionIndex)) continue;
            const FSkelMeshSourceSectionUserData* User = LodModel.UserSectionsData.Find(Section.OriginalDataSectionIndex);
            // RemoveMeshSection uses FindChecked and refuses sections with clothing bindings.
            if (Section.OriginalDataSectionIndex < 0 || !User || User->HasClothingData() || Section.HasClothingData()) return -1;
            if (!Section.bDisabled || !User->bDisabled) Changes.Emplace(LOD, Index);
        }
    }
    if (Changes.IsEmpty()) return 0;

    // Prevalidation above finishes before any edit. Nested RemoveMeshSection scopes rebuild only once.
    {
        FScopedSkeletalMeshPostEditChange Scope(SkeletalMesh);
        for (const FIntPoint& Change : Changes) SkeletalMesh->RemoveMeshSection(Change.X, Change.Y);
    }
    return Changes.Num();
}

UMaterialExpression* UNewEnemyAnimationLibrary::GetMaterialAttributesInput(UMaterial* Material, FString& OutputName)
{
    OutputName = UMaterialEditingLibrary::GetMaterialPropertyInputNodeOutputName(Material, MP_MaterialAttributes);
    return UMaterialEditingLibrary::GetMaterialPropertyInputNode(Material, MP_MaterialAttributes);
}

bool UNewEnemyAnimationLibrary::ConnectMaterialAttributes(UMaterial* Material, UMaterialExpression* Expression, const FString& OutputName)
{
    return Material && Expression && Expression->GetOuter() == Material &&
        UMaterialEditingLibrary::ConnectMaterialProperty(Expression, OutputName, MP_MaterialAttributes);
}

UMaterialExpression* UNewEnemyAnimationLibrary::CreateEmissiveAttributeOverride(UMaterial* Material, int32 X, int32 Y)
{
    if (!Material) return nullptr;
    auto* Expression = Cast<UMaterialExpressionSetMaterialAttributes>(UMaterialEditingLibrary::CreateMaterialExpression(
        Material, UMaterialExpressionSetMaterialAttributes::StaticClass(), X, Y));
    if (Expression)
    {
        // Initialize the GUID list and its matching input together. Python property edits update them separately.
        Expression->AttributeSetTypes = {FMaterialAttributeDefinitionMap::GetID(MP_EmissiveColor)};
        Expression->Inputs.SetNum(2);
    }
    return Expression;
}

bool UNewEnemyAnimationLibrary::ReplaceRangedCombatBranch(UBehaviorTree* Tree,
    TSubclassOf<UBTTaskNode> TaskClass, UBlackboardData* Blackboard)
{
    UBehaviorTreeGraph* Graph = Tree ? Cast<UBehaviorTreeGraph>(Tree->BTGraph) : nullptr;
    FByteProperty* AbortMode = FindFProperty<FByteProperty>(UBTDecorator::StaticClass(), TEXT("FlowAbortMode"));
    if (!Tree || !Tree->RootNode || Tree->RootNode->Children.IsEmpty() || !Graph || !TaskClass || !Blackboard ||
        !AbortMode || TaskClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists) || Graph->IsLocked())
    {
        return false;
    }

    FBTCompositeChild& Combat = Tree->RootNode->Children[0];
    UBTNode* Existing = Combat.ChildTask ? static_cast<UBTNode*>(Combat.ChildTask.Get()) : Combat.ChildComposite.Get();
    const bool bAlreadyReplaced = Existing && Existing->IsA(TaskClass.Get());
    if (!Existing || (!bAlreadyReplaced && Existing->GetNodeName() != TEXT("CombatState"))) return false;

    UBehaviorTreeGraphNode* OldGraphNode = nullptr;
    for (UEdGraphNode* Node : Graph->Nodes)
    {
        UBehaviorTreeGraphNode* Candidate = Cast<UBehaviorTreeGraphNode>(Node);
        if (Candidate && Candidate->NodeInstance == Existing)
        {
            OldGraphNode = Candidate;
            break;
        }
    }
    if (!OldGraphNode || !OldGraphNode->GetInputPin()) return false;

    Tree->Modify();
    Graph->Modify();
    Tree->RootNode->Modify();
    Tree->BlackboardAsset = Blackboard;
    // The editor ROOT owns a second Blackboard reference and writes it back on edits/undo.
    for (UEdGraphNode* Node : Graph->Nodes)
    {
        if (UBehaviorTreeGraphNode_Root* Root = Cast<UBehaviorTreeGraphNode_Root>(Node))
        {
            Root->Modify();
            Root->BlackboardAsset = Blackboard;
        }
    }
    // The combat task stays active. Reevaluate both on entering combat and when State changes away from it.
    for (UBTDecorator* Decorator : Combat.Decorators)
    {
        if (Decorator)
        {
            Decorator->Modify();
            AbortMode->SetPropertyValue_InContainer(Decorator, static_cast<uint8>(EBTFlowAbortMode::Both));
        }
    }

    if (bAlreadyReplaced)
    {
        Graph->UpdateAsset(UBehaviorTreeGraph::ClearDebuggerFlags);
        Tree->MarkPackageDirty();
        return Tree->RootNode && Tree->RootNode->Children.IsValidIndex(0) &&
            Tree->RootNode->Children[0].ChildTask.Get() == Existing;
    }

    // Collect only the old combat subtree. Patrol nodes, their services, and graph positions stay in place.
    TSet<UBehaviorTreeGraphNode*> RemovedNodes;
    TArray<UBehaviorTreeGraphNode*> Pending { OldGraphNode };
    while (!Pending.IsEmpty())
    {
        UBehaviorTreeGraphNode* Node = Pending.Pop(EAllowShrinking::No);
        if (!Node || RemovedNodes.Contains(Node)) continue;
        RemovedNodes.Add(Node);
        for (UEdGraphPin* Pin : Node->Pins)
        {
            if (Pin && Pin->Direction == EGPD_Output)
            {
                for (UEdGraphPin* Link : Pin->LinkedTo)
                {
                    if (Link) Pending.Add(Cast<UBehaviorTreeGraphNode>(Link->GetOwningNode()));
                }
            }
        }
    }

    Graph->LockUpdates();
    UBTTaskNode* Task = NewObject<UBTTaskNode>(Tree, TaskClass, NAME_None, RF_Transactional);
    FGraphNodeCreator<UBehaviorTreeGraphNode_Task> Builder(*Graph);
    UBehaviorTreeGraphNode_Task* NewGraphNode = Builder.CreateNode();
    NewGraphNode->NodeInstance = Task;
    NewGraphNode->NodePosX = OldGraphNode->NodePosX;
    NewGraphNode->NodePosY = OldGraphNode->NodePosY;
    NewGraphNode->NodeComment = OldGraphNode->NodeComment;
    Builder.Finalize();
    NewGraphNode->UpdateNodeClassData();

    // Decorators belong to the incoming edge; transfer them rather than copying their conditions.
    OldGraphNode->Modify();
    NewGraphNode->Decorators = OldGraphNode->Decorators;
    for (UBehaviorTreeGraphNode* Decorator : NewGraphNode->Decorators)
    {
        if (Decorator)
        {
            Decorator->Modify();
            OldGraphNode->SubNodes.Remove(Decorator);
            Decorator->ParentNode = NewGraphNode;
            NewGraphNode->SubNodes.Add(Decorator);
        }
    }
    OldGraphNode->Decorators.Reset();
    UEdGraphPin* OldInput = OldGraphNode->GetInputPin();
    TArray<UEdGraphPin*> ParentPins = OldInput->LinkedTo;
    for (UEdGraphPin* ParentPin : ParentPins)
    {
        if (ParentPin) ParentPin->GetOwningNode()->Modify();
    }
    OldInput->BreakAllPinLinks();
    for (UEdGraphPin* ParentPin : ParentPins)
    {
        if (ParentPin) ParentPin->MakeLinkTo(NewGraphNode->GetInputPin());
    }

    Combat.ChildComposite = nullptr;
    Combat.ChildTask = Task;
    for (UBehaviorTreeGraphNode* Node : RemovedNodes)
    {
        Node->Modify();
        Node->BreakAllNodeLinks();
        Graph->RemoveNode(Node);
    }
    Graph->UnlockUpdates(); // Rebuild runtime execution order from the graph, including key-selector initialization.
    Graph->UpdateClassData();
    Graph->RemoveOrphanedNodes();
    Graph->NotifyGraphChanged();
    Tree->MarkPackageDirty();
    return Tree->RootNode && Tree->RootNode->Children.IsValidIndex(0) &&
        Tree->RootNode->Children[0].ChildTask == Task && !Tree->RootNode->Children[0].ChildComposite;
}

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
