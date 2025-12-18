// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/SceneCapture2D.h"
#include "MinimapSceneCapture2D.generated.h"

class UMinimapViewModel;
;
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
    void InitializeCaptureComponent();
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

private:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    float BorderRatio = 1.1f;

    float WorldCaptureLength = 0;
    FVector2D WorldCapturePoint = FVector2D::ZeroVector;
};
