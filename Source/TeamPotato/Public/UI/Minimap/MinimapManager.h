// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MinimapManager.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UMinimapManager : public UObject
{
	GENERATED_BODY()
	
public:
    void InitializeMinimapManager(UTextureRenderTarget2D* InRenderTarget,
        FVector2D InMinPoint, FVector2D InMaxPoint, float InOrthoWidth);
    
    // 월드 좌표를 미니맵 UV 좌표로 변환
    FVector2D WorldToMinimapUV(const FVector2D& InWorldLocation2D) const;

protected:


private:
    // 미니맵 머티리얼 인스턴스
    TObjectPtr<UMaterialInstanceDynamic> MinimapMaterial = nullptr;

    // 찍고 있는 월드 좌표 범위
    FVector2D WorldMinPoint = FVector2D::ZeroVector;
    FVector2D WorldMaxPoint = FVector2D::ZeroVector;

    // 오쏘의 한변 길이
    float OrthoWidth = 0.f;
};
