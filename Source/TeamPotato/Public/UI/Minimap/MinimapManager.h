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

    // --- 포그 오브 워 업데이트 ---
    void RevealArea(FVector2D UV, float Radius);

    void UpdateFogTexture();

private:
    // 미니맵 머티리얼 인스턴스
    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> MinimapMaterial = nullptr;

    // 찍고 있는 월드 좌표 범위
    UPROPERTY()
    FVector2D WorldMinPoint = FVector2D::ZeroVector;

    // 오쏘의 한변 길이
    UPROPERTY()
    float OrthoWidth = 0.f;

    // 포그 오브 워 관련
    UPROPERTY()
    TObjectPtr<UTexture2D> FogTexture = nullptr;

    // 플레이어가 밝혀낸 영역 데이터
    UPROPERTY()
    TArray<FColor> FogData;

    // 포그 텍스처 해상도
    UPROPERTY()
    int32 FogResolution = 128;

    // 포그 데이터가 변경되었는지 여부
    UPROPERTY()
    bool bFogDirty = false;
};
