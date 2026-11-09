#include "EnemyDamageMigrationLibrary.h"

#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraphSchema_K2.h"
#include "Engine/Blueprint.h"
#include "GameFramework/Actor.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Self.h"
#include "K2Node_SpawnActorFromClass.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "UObject/UnrealType.h"

namespace
{
    TArray<UK2Node_CallFunction*> FindLegacyCalls(UBlueprint* Blueprint)
    {
        TArray<UK2Node_CallFunction*> Calls;
        if (!Blueprint) return Calls;
        const UFunction* Legacy = UGameplayStatics::StaticClass()->FindFunctionByName(
            GET_FUNCTION_NAME_CHECKED(UGameplayStatics, ApplyDamage));
        if (!Legacy) return Calls;
        TArray<UEdGraph*> Graphs;
        // Blueprint::GetAllGraphs traverses editable function/macro/event/interface graphs,
        // not IntermediateGeneratedGraphs/ExecuteUbergraph compiler output.
        Blueprint->GetAllGraphs(Graphs);
        for (UEdGraph* Graph : Graphs)
        {
            if (!Graph) continue;
            for (UEdGraphNode* Node : Graph->Nodes)
                if (auto* Call = Cast<UK2Node_CallFunction>(Node))
                    if (Call->GetTargetFunction() == Legacy) Calls.AddUnique(Call);
        }
        return Calls;
    }

    FName MapPin(FName Name)
    {
        if (Name == TEXT("DamagedActor")) return TEXT("Target");
        if (Name == TEXT("BaseDamage")) return TEXT("Damage");
        if (Name == TEXT("EventInstigator")) return TEXT("InstigatorController");
        if (Name == TEXT("DamageTypeClass")) return TEXT("DamageType");
        return Name;
    }
}

bool UEnemyDamageMigrationLibrary::IsBlueprintCompiled(UBlueprint* Blueprint)
{
    return Blueprint && (Blueprint->Status == BS_UpToDate || Blueprint->Status == BS_UpToDateWithWarnings);
}

int32 UEnemyDamageMigrationLibrary::CountLegacyDamageCalls(UBlueprint* Blueprint)
{
    return Blueprint ? FindLegacyCalls(Blueprint).Num() : -1;
}

int32 UEnemyDamageMigrationLibrary::MigrateLegacyDamageCalls(UBlueprint* Blueprint)
{
    if (!Blueprint || !IsInGameThread()) return -1;
    const TArray<UK2Node_CallFunction*> OldNodes = FindLegacyCalls(Blueprint);
    if (OldNodes.IsEmpty()) return 0;
    UClass* CombatLibrary = FindObject<UClass>(nullptr, TEXT("/Script/TeamPotato.CombatFunctionLibrary"));
    UFunction* DamageFunction = CombatLibrary ? CombatLibrary->FindFunctionByName(TEXT("ApplyCombatDamage")) : nullptr;
    if (!DamageFunction || !DamageFunction->HasAllFunctionFlags(FUNC_Static | FUNC_BlueprintCallable)) return -1;

    TArray<UK2Node_CallFunction*> NewNodes;
    TMap<UEdGraphPin*, UEdGraphPin*> PinMap;
    auto ValidationFailure = [&]()
    {
        // These nodes were never added to a graph's Nodes array. Source assets are unchanged.
        for (UK2Node_CallFunction* Node : NewNodes) Node->MarkAsGarbage();
        UE_LOG(LogTemp, Error, TEXT("Enemy damage migration validation failed: %s"), *Blueprint->GetPathName());
        return -1;
    };

    // Allocate unattached replacement nodes and validate every source pin before changing any wires.
    for (UK2Node_CallFunction* Old : OldNodes)
    {
        UEdGraph* Graph = Old->GetGraph();
        if (!Graph || !Cast<UEdGraphSchema_K2>(Graph->GetSchema())) return ValidationFailure();
        auto* Replacement = NewObject<UK2Node_CallFunction>(Graph, NAME_None, RF_Transient);
        NewNodes.Add(Replacement);
        Replacement->SetFromFunction(DamageFunction);
        Replacement->AllocateDefaultPins();
        for (UEdGraphPin* Pin : Old->Pins)
        {
            if (!Pin || Pin->bOrphanedPin || Pin->SubPins.Num() > 0) return ValidationFailure();
            // Static library target pins name different libraries, and must never carry data wires.
            if (Pin->PinName == UEdGraphSchema_K2::PN_Self)
            {
                if (!Pin->LinkedTo.IsEmpty()) return ValidationFailure();
                continue;
            }
            UEdGraphPin* NewPin = Replacement->FindPin(MapPin(Pin->PinName), Pin->Direction);
            if (!NewPin || NewPin->PinType != Pin->PinType)
            {
                UE_LOG(LogTemp, Error, TEXT("Incompatible damage migration pin %s.%s"),
                    *Old->GetPathName(), *Pin->PinName.ToString());
                return ValidationFailure();
            }
            for (UEdGraphPin* Linked : Pin->LinkedTo)
                if (!Linked || Linked->bOrphanedPin || Linked->Direction == Pin->Direction) return ValidationFailure();
            PinMap.Add(Pin, NewPin);
        }
        // A changed target signature must fail instead of silently adding unmapped arguments.
        if (Replacement->Pins.Num() != Old->Pins.Num()) return ValidationFailure();
    }

    TArray<TPair<UEdGraphPin*, UEdGraphPin*>> NewLinks;
    for (const auto& Pair : PinMap)
        for (UEdGraphPin* Linked : Pair.Key->LinkedTo)
            NewLinks.Emplace(Pair.Value, PinMap.Contains(Linked) ? PinMap[Linked] : Linked);

    Blueprint->Modify();
    for (int32 Index = 0; Index < OldNodes.Num(); ++Index)
    {
        UK2Node_CallFunction* Old = OldNodes[Index];
        UK2Node_CallFunction* Replacement = NewNodes[Index];
        Old->GetGraph()->Modify();
        Old->Modify();
        Replacement->ClearFlags(RF_Transient);
        Replacement->SetFlags(RF_Transactional);
        Replacement->CreateNewGuid();
        Replacement->NodePosX = Old->NodePosX;
        Replacement->NodePosY = Old->NodePosY;
        Replacement->NodeComment = Old->NodeComment;
        Replacement->bCommentBubblePinned = Old->bCommentBubblePinned;
        Replacement->bCommentBubbleVisible = Old->bCommentBubbleVisible;
        Replacement->bCommentBubbleMakeVisible = Old->bCommentBubbleMakeVisible;
        Replacement->AdvancedPinDisplay = Old->AdvancedPinDisplay;
        Replacement->SetEnabledState(Old->GetDesiredEnabledState(), Old->HasUserSetTheEnabledState());
        Old->GetGraph()->AddNode(Replacement, false, false);
    }
    for (const auto& Pair : PinMap)
    {
        UEdGraphPin* OldPin = Pair.Key;
        UEdGraphPin* NewPin = Pair.Value;
        NewPin->DefaultValue = OldPin->DefaultValue;
        NewPin->DefaultObject = OldPin->DefaultObject;
        NewPin->DefaultTextValue = OldPin->DefaultTextValue;
        // An unspecified old causer now uses ApplyCombatDamage's DefaultToSelf metadata.
        // Explicit causer objects/connections are preserved, including pooled projectile context.
        if (OldPin->PinName == TEXT("DamageCauser") && OldPin->LinkedTo.IsEmpty() && !OldPin->DefaultObject)
            NewPin->DefaultValue.Reset();
        for (UEdGraphPin* Linked : OldPin->LinkedTo) Linked->GetOwningNode()->Modify();
        OldPin->BreakAllPinLinks(false);
    }
    // Pin types/directions are identical to the original valid connections. Direct rewiring avoids
    // schema auto-conversions or breaking other exec consumers while two old calls are replaced.
    for (const auto& Link : NewLinks) Link.Key->MakeLinkTo(Link.Value);
    for (UK2Node_CallFunction* Old : OldNodes) FBlueprintEditorUtils::RemoveNode(Blueprint, Old, true);
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
    return NewNodes.Num();
}

int32 UEnemyDamageMigrationLibrary::ConnectMissingSpawnOwners(UBlueprint* Blueprint)
{
    if (!Blueprint || !IsInGameThread() || !Blueprint->ParentClass ||
        !Blueprint->ParentClass->IsChildOf(AActor::StaticClass())) return -1;
    TArray<UEdGraph*> Graphs;
    Blueprint->GetAllGraphs(Graphs);
    TArray<UEdGraphPin*> Owners;
    TMap<UEdGraph*, UK2Node_Self*> SelfNodes;
    TArray<UK2Node_Self*> NewSelfNodes;
    auto ValidationFailure = [&]()
    {
        for (UK2Node_Self* Node : NewSelfNodes) Node->MarkAsGarbage();
        UE_LOG(LogTemp, Error, TEXT("Enemy spawn owner migration validation failed: %s"), *Blueprint->GetPathName());
        return -1;
    };

    for (UEdGraph* Graph : Graphs)
    {
        if (!Graph) continue;
        for (UEdGraphNode* Node : Graph->Nodes)
        {
            auto* Spawn = Cast<UK2Node_SpawnActorFromClass>(Node);
            if (!Spawn) continue;
            const UClass* SpawnClass = Spawn->GetClassToSpawn();
            if (!SpawnClass || !SpawnClass->GetPathName().StartsWith(TEXT("/Game/Enemy/Blueprint/Bullet/"))) continue;
            UEdGraphPin* Owner = Spawn->FindPin(TEXT("Owner"), EGPD_Input);
            if (!Owner || Owner->bOrphanedPin) return ValidationFailure();
            if (!Owner->LinkedTo.IsEmpty() || Owner->DefaultObject ||
                (!Owner->DefaultValue.IsEmpty() && Owner->DefaultValue != TEXT("None"))) continue;
            const auto* Schema = Cast<UEdGraphSchema_K2>(Graph->GetSchema());
            if (!Schema) return ValidationFailure();

            UK2Node_Self* Self = SelfNodes.FindRef(Graph);
            if (!Self)
            {
                // A single self reference per graph keeps larger boss pattern graphs readable.
                for (UEdGraphNode* Candidate : Graph->Nodes)
                    if (auto* Existing = Cast<UK2Node_Self>(Candidate))
                        if (Existing->IsNodeEnabled()) { Self = Existing; break; }
                if (!Self)
                {
                    Self = NewObject<UK2Node_Self>(Graph, NAME_None, RF_Transient);
                    Self->AllocateDefaultPins();
                    Self->NodePosX = Spawn->NodePosX - 180;
                    Self->NodePosY = Spawn->NodePosY + 240;
                    NewSelfNodes.Add(Self);
                }
                SelfNodes.Add(Graph, Self);
            }
            UEdGraphPin* SelfPin = Self->FindPin(UEdGraphSchema_K2::PN_Self, EGPD_Output);
            if (!SelfPin || !Schema->CanCreateConnection(SelfPin, Owner).CanSafeConnect()) return ValidationFailure();
            Owners.AddUnique(Owner);
        }
    }
    if (Owners.IsEmpty()) return 0;

    Blueprint->Modify();
    for (UK2Node_Self* Self : NewSelfNodes)
    {
        Self->GetGraph()->Modify();
        Self->ClearFlags(RF_Transient);
        Self->SetFlags(RF_Transactional);
        Self->CreateNewGuid();
        Self->GetGraph()->AddNode(Self, false, false);
    }
    for (UEdGraphPin* Owner : Owners)
    {
        UEdGraphNode* Spawn = Owner->GetOwningNode();
        Spawn->GetGraph()->Modify();
        Spawn->Modify();
        UK2Node_Self* Self = SelfNodes.FindChecked(Spawn->GetGraph());
        Self->Modify();
        Owner->DefaultValue.Reset();
        Self->FindPinChecked(UEdGraphSchema_K2::PN_Self)->MakeLinkTo(Owner);
    }
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);
    return Owners.Num();
}
