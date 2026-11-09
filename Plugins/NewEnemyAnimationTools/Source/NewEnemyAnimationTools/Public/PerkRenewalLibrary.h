#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PerkRenewalLibrary.generated.h"

class UBlueprint;
class UDataTable;
class UGameplayEffect;
class UPerkDataAsset;
class UTexture2D;

// Editor-only authoring helpers. Runtime assets have no dependency on this plugin.
UCLASS()
class NEWENEMYANIMATIONTOOLS_API UPerkRenewalLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Editor|Perk Renewal")
    static bool ConfigurePerkGameplayEffect(UBlueprint* Blueprint, FName AttributeName, float Magnitude,
        bool bAdditive, FName GrantedTagName);

    UFUNCTION(BlueprintCallable, Category = "Editor|Perk Renewal")
    static bool ConfigurePerkData(UPerkDataAsset* Perk, TSubclassOf<UGameplayEffect> EffectClass,
        FText Name, FText Description, UTexture2D* Icon, float DisplayAmount);

    UFUNCTION(BlueprintCallable, Category = "Editor|Perk Renewal")
    static bool ReplacePerkSelectionTable(UDataTable* Table, const TArray<UPerkDataAsset*>& Perks);

    UFUNCTION(BlueprintCallable, Category = "Editor|Perk Renewal")
    static bool ValidatePerkSelectionTable(UDataTable* Table);
};
