// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Minimap/MinimapManager.h"
#include "Common/MyGameSettings.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"

void UMinimapManager::InitializeMinimapManager(UTextureRenderTarget2D* InRenderTarget, FVector2D InMinPoint, float InOrthoWidth)
{
    if (!InRenderTarget) return;

    // 게임 설정에서 베이스 머티리얼 로드
    UMaterialInterface* BaseMaterial = 
        UMyGameSettings::Get()->MinimapBaseMaterial.LoadSynchronous();

    UTexture* PlayerIconTexture = 
        UMyGameSettings::Get()->MinimapPlayerIcon.LoadSynchronous();

    if (!BaseMaterial) return;
    
    // Material 생성
    MinimapMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);

    // RenderTarget을 텍스처 파라미터로 설정
    MinimapMaterial->SetTextureParameterValue(TEXT("MinimapTexture"), InRenderTarget);

    if (!PlayerIconTexture) return;
    // 플레이어 아이콘 텍스처 설정
    MinimapMaterial->SetTextureParameterValue(TEXT("PlayerIconTexture"), PlayerIconTexture);

    MinimapMaterial->SetScalarParameterValue(TEXT("PlayerIconSize"), 0.05f);

    WorldMinPoint = InMinPoint;
    OrthoWidth = InOrthoWidth;
}

FVector2D UMinimapManager::WorldToMinimapUV(const FVector2D& InWorldLocation2D) const
{
    if (OrthoWidth <= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("MinimapViewModel::ConvertWorldToMinimapCoordinates - OrthoWidth, WorldMinPoint, or WorldMaxPoint is not set properly."));
        return FVector2D(0.5f, 0.5f);
    }

    // 월드 렌더 영역 내에서의 입력 좌표의 상대적인 위치 계산
    FVector2D RelativePosition = InWorldLocation2D - WorldMinPoint;

    // 상대적인 위치를 UV 좌표로 변환 (0~1 범위)
    FVector2D UV;
    UV.X = RelativePosition.Y / OrthoWidth;
    UV.Y = 1 -(RelativePosition.X / OrthoWidth);
    
    // 혹시 범위 벗어나는 경우 클램프
    UV.X = FMath::Clamp(UV.X, 0.0f, 1.0f);
    UV.Y = FMath::Clamp(UV.Y, 0.0f, 1.0f);
    
    return UV;
}

void UMinimapManager::UpdatePlayerPosition(const FVector& InWorldLocation, float InYaw)
{
    if (!MinimapMaterial) return;

    FVector2D PlayerUV = WorldToMinimapUV(FVector2D(InWorldLocation.X, InWorldLocation.Y));

    //UE_LOG(LogTemp, Log, TEXT("WorldMin: %s, OrthoWidth: %f"), *WorldMinPoint.ToString(), OrthoWidth);
    //UE_LOG(LogTemp, Log, TEXT("PlayerWorld: %s -> UV: %s"), *InWorldLocation.ToString(), *PlayerUV.ToString());

    // Material 파라미터 업데이트
    MinimapMaterial->SetScalarParameterValue(TEXT("PlayerPosX"), PlayerUV.X);
    MinimapMaterial->SetScalarParameterValue(TEXT("PlayerPosY"), PlayerUV.Y);
    MinimapMaterial->SetScalarParameterValue(TEXT("PlayerRotation"), InYaw/360.f);
}