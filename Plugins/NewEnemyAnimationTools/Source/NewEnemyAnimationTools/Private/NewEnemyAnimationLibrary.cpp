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
