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

    // 포그 오브 워 설정
    FogTexture = UTexture2D::CreateTransient(FogResolution, FogResolution, EPixelFormat::PF_B8G8R8A8);
    FogTexture->UpdateResource();

    FogData.Init(FColor::Black, FogResolution * FogResolution);
    MinimapMaterial->SetTextureParameterValue(TEXT("FogTexture"), FogTexture);
    MinimapMaterial->SetScalarParameterValue(TEXT("ViewRadius"), 0.15f);
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

    RevealArea(PlayerUV, 0.1f);
}

void UMinimapManager::RevealArea(FVector2D UV, float Radius)
{
    int32 CenterX = FMath::RoundToInt(UV.X * FogResolution);
    int32 CenterY = FMath::RoundToInt(UV.Y * FogResolution);
    int32 RadiusPixels = FMath::RoundToInt(Radius * FogResolution);

    // 원형 영역의 픽셀을 흰색으로 설정
    for (int32 Y = -RadiusPixels; Y <= RadiusPixels; Y++)
    {
        for (int32 X = -RadiusPixels; X <= RadiusPixels; X++)
        {
            if (X * X + Y * Y <= RadiusPixels * RadiusPixels)
            {
                int32 PX = FMath::Clamp(CenterX + X, 0, FogResolution - 1);
                int32 PY = FMath::Clamp(CenterY + Y, 0, FogResolution - 1);

                FogData[PY * FogResolution + PX] = FColor::White;
                bFogDirty = true;
            }
        }
    }

    if(bFogDirty)
    {
        UpdateFogTexture();
        bFogDirty = false;
    }
}

void UMinimapManager::UpdateFogTexture()
{
    if (!FogTexture) return;

    // 텍스처 데이터 업데이트
    void* TextureData = FogTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
    FMemory::Memcpy(TextureData, FogData.GetData(), FogData.Num() * sizeof(FColor));
    FogTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
    FogTexture->UpdateResource();
}
