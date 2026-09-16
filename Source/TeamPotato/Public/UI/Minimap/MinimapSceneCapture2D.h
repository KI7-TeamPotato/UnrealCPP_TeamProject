// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/SceneCapture2D.h"
#include "MinimapSceneCapture2D.generated.h"

// 캡처한 미니맵 렌더 타깃과 

class UMinimapViewModel;
class UMinimapManager;
class UMaterialInterface;
class UTexture;
struct FStreamableHandle;

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API AMinimapSceneCapture2D : public ASceneCapture2D
{
	GENERATED_BODY()
public:
    AMinimapSceneCapture2D();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
    UFUNCTION()
    void OnMinimapCapture(FVector2D InMinPoint, FVector2D InMaxPoint);
	
private:
    // --- 렌더 설정(렌더 방식) 함수 ---
    void InitializeCaptureComponent();

    // --- 렌더 타깃 초기화 함수(크기) ---
    void InitializeRenderTarget();

    // --- 미니맵 에셋을 게임 플레이를 막지 않고 미리 로드 ---
    void RequestMinimapAssets();
    void HandleMinimapAssetsLoaded();

    // --- 에셋이 준비된 뒤 실제 캡처 및 매니저 초기화 ---
    void CaptureMinimap(FVector2D InMinPoint, FVector2D InMaxPoint);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USceneCaptureComponent2D> CaptureComp;

    // --- 미니맵 상태 뷰모델 ---
    UPROPERTY()
    TObjectPtr<UMinimapViewModel> MinimapViewModel = nullptr;

    // -- 타깃 렌더 X,Y 사이즈 --
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FVector2D RenderTargetSize = FVector2D(800.0f, 800.0f);

    // --- 미니맵 매니저 ---
    UPROPERTY()
    TObjectPtr<UMinimapManager> MinimapManager = nullptr;

    UPROPERTY()
    TObjectPtr<UMaterialInterface> MinimapBaseMaterial = nullptr;

    UPROPERTY()
    TObjectPtr<UTexture> MinimapPlayerIcon = nullptr;

private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    float BorderRatio = 1.1f;

    float CaptureOrthoWidth = 0;
    FVector2D WorldMinPoint = FVector2D::ZeroVector;
    FVector2D WorldMaxPoint = FVector2D::ZeroVector;

    TSharedPtr<FStreamableHandle> MinimapAssetLoadHandle;
    bool bHasPendingCapture = false;
    FVector2D PendingMinPoint = FVector2D::ZeroVector;
    FVector2D PendingMaxPoint = FVector2D::ZeroVector;
};
