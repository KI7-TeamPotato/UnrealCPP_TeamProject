// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/SceneCapture2D.h"
#include "MinimapSceneCapture2D.generated.h"

// 캡처한 미니맵 렌더 타깃과 

class UMinimapViewModel;
class UMinimapManager;

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
    
    UFUNCTION()
    void OnMinimapCapture(FVector2D InMinPoint, FVector2D InMaxPoint);
	
private:
    // --- 렌더 설정(렌더 방식) 함수 ---
    void InitializeCaptureComponent();

    // --- 렌더 타깃 초기화 함수(크기) ---
    void InitializeRenderTarget();

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

private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    float BorderRatio = 1.1f;

    float CaptureOrthoWidth = 0;
    FVector2D WorldMinPoint = FVector2D::ZeroVector;
    FVector2D WorldMaxPoint = FVector2D::ZeroVector;
};
