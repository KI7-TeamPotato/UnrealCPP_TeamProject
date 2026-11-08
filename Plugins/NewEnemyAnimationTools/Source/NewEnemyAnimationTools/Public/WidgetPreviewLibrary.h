#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WidgetPreviewLibrary.generated.h"

class UBlueprint;
class UUserWidget;
class UWidget;

/** Editor-only widget authoring and image previews. Assets are saved explicitly by the caller. */
UCLASS()
class NEWENEMYANIMATIONTOOLS_API UWidgetPreviewLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Editor|Widget Preview")
    static UUserWidget* CreatePreviewWidget(UObject* WorldContextObject, TSubclassOf<UUserWidget> WidgetClass);

    UFUNCTION(BlueprintCallable, Category = "Editor|Widget Preview")
    static UWidget* GetWidgetBlueprintRoot(UBlueprint* Blueprint);

    // Creates a transactional template owned by the Blueprint's WidgetTree. The caller attaches it to a panel.
    UFUNCTION(BlueprintCallable, Category = "Editor|Widget Preview", meta = (DeterminesOutputType = "WidgetClass"))
    static UWidget* CreateWidgetTemplate(UBlueprint* Blueprint, TSubclassOf<UWidget> WidgetClass, FName WidgetName, bool bIsVariable = false);

    UFUNCTION(BlueprintCallable, Category = "Editor|Widget Preview")
    static UWidget* GetWidgetByName(UUserWidget* Widget, FName WidgetName);

    UFUNCTION(BlueprintCallable, Category = "Editor|Widget Preview")
    static bool MarkWidgetBlueprintModified(UBlueprint* Blueprint);

    // Requires Slate and a rendering RHI; use a normal editor with -RenderOffscreen, not -NullRHI.
    UFUNCTION(BlueprintCallable, Category = "Editor|Widget Preview")
    static bool RenderWidgetPreview(UUserWidget* Widget, FIntPoint Size, const FString& OutputPath);
};
