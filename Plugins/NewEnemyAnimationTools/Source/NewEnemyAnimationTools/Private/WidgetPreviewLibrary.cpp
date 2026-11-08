#include "WidgetPreviewLibrary.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Widget.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Framework/Application/SlateApplication.h"
#include "HAL/FileManager.h"
#include "ImageUtils.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Misc/App.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "RenderingThread.h"
#include "RHI.h"
#include "Slate/WidgetRenderer.h"
#include "TextureResource.h"
#include "UObject/StrongObjectPtr.h"
#include "WidgetBlueprint.h"

UUserWidget* UWidgetPreviewLibrary::CreatePreviewWidget(UObject* WorldContextObject, TSubclassOf<UUserWidget> WidgetClass)
{
    return WorldContextObject && WidgetClass ? UWidgetBlueprintLibrary::Create(WorldContextObject, WidgetClass, nullptr) : nullptr;
}

UWidget* UWidgetPreviewLibrary::GetWidgetBlueprintRoot(UBlueprint* Blueprint)
{
    const UWidgetBlueprint* WidgetBlueprint = Cast<UWidgetBlueprint>(Blueprint);
    return WidgetBlueprint && WidgetBlueprint->WidgetTree ? WidgetBlueprint->WidgetTree->RootWidget.Get() : nullptr;
}

UWidget* UWidgetPreviewLibrary::CreateWidgetTemplate(UBlueprint* Blueprint, TSubclassOf<UWidget> WidgetClass, FName WidgetName, bool bIsVariable)
{
    UWidgetBlueprint* WidgetBlueprint = Cast<UWidgetBlueprint>(Blueprint);
    if (!WidgetBlueprint || !WidgetBlueprint->WidgetTree || !WidgetClass ||
        WidgetClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists)) return nullptr;
    UWidgetTree* Tree = WidgetBlueprint->WidgetTree;
    if (!WidgetName.IsNone() && FindObjectFast<UObject>(Tree, WidgetName)) return nullptr;
    Tree->Modify();
    UWidget* NewWidget = Tree->ConstructWidget<UWidget>(WidgetClass, WidgetName);
    if (NewWidget) NewWidget->bIsVariable = bIsVariable;
    return NewWidget;
}

UWidget* UWidgetPreviewLibrary::GetWidgetByName(UUserWidget* Widget, FName WidgetName)
{
    return IsValid(Widget) ? Widget->GetWidgetFromName(WidgetName) : nullptr;
}

bool UWidgetPreviewLibrary::MarkWidgetBlueprintModified(UBlueprint* Blueprint)
{
    UWidgetBlueprint* WidgetBlueprint = Cast<UWidgetBlueprint>(Blueprint);
    if (!WidgetBlueprint) return false;
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(WidgetBlueprint);
    return true;
}

bool UWidgetPreviewLibrary::RenderWidgetPreview(UUserWidget* Widget, FIntPoint Size, const FString& OutputPath)
{
    if (!IsInGameThread() || !IsValid(Widget) || !FSlateApplication::IsInitialized() ||
        !FApp::CanEverRender() || GUsingNullRHI || Size.X <= 0 || Size.Y <= 0 ||
        Size.X > 8192 || Size.Y > 8192 || !FPaths::GetExtension(OutputPath).Equals(TEXT("png"), ESearchCase::IgnoreCase))
    {
        UE_LOG(LogTemp, Error, TEXT("Widget preview needs a valid widget, PNG path, dimensions and rendering editor."));
        return false;
    }

    const FString Filename = FPaths::ConvertRelativePathToFull(OutputPath);
    if (!IFileManager::Get().MakeDirectory(*FPaths::GetPath(Filename), true)) return false;

    TStrongObjectPtr<UUserWidget> KeepWidget(Widget);
    const TSharedRef<SWidget> SlateWidget = Widget->TakeWidget();
    const FVector2D DrawSize(Size.X, Size.Y);
    FWidgetRenderer Renderer(true, true);
    TStrongObjectPtr<UTextureRenderTarget2D> Target(FWidgetRenderer::CreateTargetFor(DrawSize, TF_Bilinear, true));
    if (!Target.IsValid()) return false;

    // Repaint without advancing gameplay or widget animations. The first paint can initialize font/brush resources.
    for (int32 Pass = 0; Pass < 3; ++Pass)
    {
        Widget->ForceLayoutPrepass();
        SlateWidget->SlatePrepass(1.0f);
        Renderer.DrawWidget(Target.Get(), SlateWidget, DrawSize, 0.0f, false);
        FlushRenderingCommands();
    }

    TArray<FColor> Pixels;
    FReadSurfaceDataFlags Flags(RCM_UNorm);
    Flags.SetLinearToGamma(false);
    if (!Target->GameThread_GetRenderTargetResource()->ReadPixels(Pixels, Flags) || Pixels.Num() != Size.X * Size.Y) return false;

    TArray64<uint8> PNG;
    FImageUtils::PNGCompressImageArray(Size.X, Size.Y, TArrayView64<const FColor>(Pixels.GetData(), Pixels.Num()), PNG);
    const bool bSaved = !PNG.IsEmpty() && FFileHelper::SaveArrayToFile(PNG, *Filename);
    if (bSaved) UE_LOG(LogTemp, Display, TEXT("WIDGET_PREVIEW_SAVED %s"), *Filename);
    return bSaved;
}
