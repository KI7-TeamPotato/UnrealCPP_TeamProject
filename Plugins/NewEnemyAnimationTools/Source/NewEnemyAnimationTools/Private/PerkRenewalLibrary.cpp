#include "PerkRenewalLibrary.h"

#include "Combat/CombatAttributeSet.h"
#include "Data/PerkDataAsset.h"
#include "Data/PerkDataTableRow.h"
#include "Engine/Blueprint.h"
#include "Engine/DataTable.h"
#include "GameplayEffect.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "GameplayEffectComponents/TargetTagRequirementsGameplayEffectComponent.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Player/Perk/GameplayEffectPerkEffect.h"
#include "UObject/UnrealType.h"

bool UPerkRenewalLibrary::ConfigurePerkGameplayEffect(UBlueprint* Blueprint, FName AttributeName,
    float Magnitude, bool bAdditive, FName GrantedTagName)
{
    if (!Blueprint || !Blueprint->GeneratedClass || !FMath::IsFinite(Magnitude)) return false;
    auto* Effect = Cast<UGameplayEffect>(Blueprint->GeneratedClass->GetDefaultObject());
    if (!Effect) return false;
    FProperty* AttributeProperty = AttributeName.IsNone() ? nullptr : FindFProperty<FProperty>(UCombatAttributeSet::StaticClass(), AttributeName);
    if (!AttributeName.IsNone() && !AttributeProperty) return false;
    const FGameplayTag GrantedTag = GrantedTagName.IsNone() ? FGameplayTag() : FGameplayTag::RequestGameplayTag(GrantedTagName, false);
    if (!GrantedTagName.IsNone() && !GrantedTag.IsValid()) return false;

    Blueprint->Modify();
    Effect->Modify();
    Effect->DurationPolicy = EGameplayEffectDurationType::Infinite;
    Effect->Period = FScalableFloat(0.0f);
    Effect->bExecutePeriodicEffectOnApplication = false;
    Effect->StackingType = EGameplayEffectStackingType::AggregateByTarget;
    Effect->StackLimitCount = 1;
    Effect->Modifiers.Reset();
    if (AttributeProperty)
    {
        FGameplayModifierInfo& Modifier = Effect->Modifiers.AddDefaulted_GetRef();
        Modifier.Attribute = FGameplayAttribute(AttributeProperty);
        Modifier.ModifierOp = bAdditive ? EGameplayModOp::Additive : EGameplayModOp::Multiplicitive;
        Modifier.ModifierMagnitude = FScalableFloat(Magnitude);
    }
    FInheritedTagContainer GrantedTags;
    if (GrantedTag.IsValid()) GrantedTags.AddTag(GrantedTag);
    Effect->FindOrAddComponent<UTargetTagsGameplayEffectComponent>().SetAndApplyTargetTagChanges(GrantedTags);
    auto& Requirements = Effect->FindOrAddComponent<UTargetTagRequirementsGameplayEffectComponent>();
    Requirements.ApplicationTagRequirements.IgnoreTags.Reset();
    Requirements.ApplicationTagRequirements.IgnoreTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("State.Dead")));
    Effect->PostEditChange();
    FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);
    FKismetEditorUtilities::CompileBlueprint(Blueprint);
    return Blueprint->Status != BS_Error;
}

bool UPerkRenewalLibrary::ConfigurePerkData(UPerkDataAsset* Perk, TSubclassOf<UGameplayEffect> EffectClass,
    FText Name, FText Description, UTexture2D* Icon, float DisplayAmount)
{
    if (!Perk || !EffectClass || !Icon || !FMath::IsFinite(DisplayAmount)) return false;
    Perk->Modify();
    Perk->PerkName = Name;
    Perk->Description = Description;
    Perk->Icon = Icon;
    Perk->PerkRarity = EPerkRarity::Common;
    auto* PerkEffect = NewObject<UGameplayEffectPerkEffect>(Perk, NAME_None, RF_Transactional);
    PerkEffect->EffectClass = EffectClass;
    auto* AmountProperty = FindFProperty<FFloatProperty>(UPerkEffectBase::StaticClass(), TEXT("Amount"));
    if (!AmountProperty) return false;
    AmountProperty->SetPropertyValue_InContainer(PerkEffect, DisplayAmount);
    Perk->PerkEffect = PerkEffect;
    Perk->MarkPackageDirty();
    return true;
}

bool UPerkRenewalLibrary::ReplacePerkSelectionTable(UDataTable* Table, const TArray<UPerkDataAsset*>& Perks)
{
    if (!Table || Table->GetRowStruct() != FPerkSelectDataTableRow::StaticStruct() || Perks.Num() != 8) return false;
    TSet<const UPerkDataAsset*> Unique;
    for (const UPerkDataAsset* Perk : Perks)
    {
        if (!IsValid(Perk) || Unique.Contains(Perk)) return false;
        Unique.Add(Perk);
    }
    Table->Modify();
    Table->EmptyTable();
    for (UPerkDataAsset* Perk : Perks)
    {
        FPerkSelectDataTableRow Row;
        Row.PerkDataAsset = Perk;
        Row.SelectWeight = 1.0f;
        Table->AddRow(Perk->GetFName(), Row);
    }
    Table->MarkPackageDirty();
    return ValidatePerkSelectionTable(Table);
}

bool UPerkRenewalLibrary::ValidatePerkSelectionTable(UDataTable* Table)
{
    if (!Table || Table->GetRowStruct() != FPerkSelectDataTableRow::StaticStruct()) return false;
    TArray<FPerkSelectDataTableRow*> Rows;
    Table->GetAllRows(TEXT("Validate renewed perks"), Rows);
    if (Rows.Num() != 8) return false;
    TSet<const UPerkDataAsset*> Unique;
    for (const FPerkSelectDataTableRow* Row : Rows)
    {
        const UPerkDataAsset* Perk = Row ? Row->PerkDataAsset.Get() : nullptr;
        if (!IsValid(Perk) || Unique.Contains(Perk) || !Perk->Icon || Perk->PerkName.IsEmpty() || Perk->Description.IsEmpty() ||
            !FMath::IsFinite(Row->SelectWeight) || Row->SelectWeight <= 0.0f) return false;
        Unique.Add(Perk);
        const auto* PerkEffect = Cast<UGameplayEffectPerkEffect>(Perk->PerkEffect);
        const UGameplayEffect* Effect = PerkEffect ? PerkEffect->EffectClass.GetDefaultObject() : nullptr;
        if (!Effect || Effect->DurationPolicy != EGameplayEffectDurationType::Infinite || Effect->Period.GetValueAtLevel(1) != 0.0f) return false;
    }
    return true;
}
