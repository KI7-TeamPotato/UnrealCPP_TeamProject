// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Minimap/MinimapManager.h"

void UMinimapManager::InitializeMinimapManager(UTextureRenderTarget2D* InRenderTarget, FVector2D InMinPoint, FVector2D InMaxPoint, float InOrthoWidth)
{
    //// Material 생성
    //MinimapMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);

    //// RenderTarget을 텍스처 파라미터로 설정 ✅ 가능!
    //MinimapMaterial->SetTextureParameterValue(TEXT("MinimapTexture"), RenderTarget);

    WorldMinPoint = InMinPoint;
    WorldMaxPoint = InMaxPoint;
    OrthoWidth = InOrthoWidth;
}

FVector2D UMinimapManager::WorldToMinimapUV(const FVector2D& InWorldLocation2D) const
{
    //if (OrthoWidth == 0 || WorldMinPoint == WorldMaxPoint)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("MinimapViewModel::ConvertWorldToMinimapCoordinates - OrthoWidth, WorldMinPoint, or WorldMaxPoint is not set properly."));
    //    return FVector2D();
    //}

    //FVector2D MinimapCoord;

    //// 월드 렌더 영역 내에서의 입력 좌표의 상대적인 위치 계산
    //FVector2D RelativePosition = InWorldLocation2D - WorldMinPoint;

    //// 입력 좌표가 월드 렌더 영역 밖에 있는지 검사
    //if (RelativePosition.X < 0 || RelativePosition.Y < 0 ||
    //    RelativePosition.X > OrthoWidth || RelativePosition.Y > OrthoWidth)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("MinimapViewModel::ConvertWorldToMinimapCoordinates - InWorldLocation2D is out of WorldRender area."));
    //    return FVector2D();
    //}

    //// 스케일링 팩터 계산(미니맵은 정사각형)
    //float SquaredMinmapLength = MinimapSize.X;
    //float ScaleFactor = SquaredMinmapLength / WorldRenderLength;

    //// 미니맵 좌표 계산
    //MinimapCoord.X = RelativePosition.X * ScaleFactor;
    //MinimapCoord.Y = RelativePosition.Y * ScaleFactor;

    return FVector2D();
}
