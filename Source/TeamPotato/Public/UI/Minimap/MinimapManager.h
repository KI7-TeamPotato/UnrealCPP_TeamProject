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
        FVector2D InMinPoint, float InOrthoWidth);
    
    // --- 월드 좌표를 미니맵 UV 좌표로 변환 ---
    FVector2D WorldToMinimapUV(const FVector2D& InWorldLocation2D) const;
   
    // --- 플레이어 위치 / 회전 업데이트 ---
    void UpdatePlayerPosition(const FVector& InWorldLocation, float InYaw);

    // --- 미니맵 머티리얼 인스턴스 반환 ---
    UFUNCTION(BlueprintPure)
    UMaterialInstanceDynamic* GetMinimapMaterial() const { return MinimapMaterial; }

private:
    // 미니맵 머티리얼 인스턴스
    TObjectPtr<UMaterialInstanceDynamic> MinimapMaterial = nullptr;

    // 찍고 있는 월드 좌표 범위
    FVector2D WorldMinPoint = FVector2D::ZeroVector;

    // 오쏘의 한변 길이
    float OrthoWidth = 0.f;
};
