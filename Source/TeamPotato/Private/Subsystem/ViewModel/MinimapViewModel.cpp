// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ViewModel/MinimapViewModel.h"

// --- 맵 생성시에 미니맵 캡처 요청 함수 ---
void UMinimapViewModel::RequestMinimapCapture(FVector2D InMinPoint, FVector2D InMaxPoint)
{
    if (OnMinimapCaptureRequested.IsBound())
    {
        OnMinimapCaptureRequested.Broadcast(InMinPoint, InMaxPoint);
    }
}

// --- 미니맵 플레이어 위치 변경 함수 ---
void UMinimapViewModel::RequestPlayerMinimapCoordinateUpdate(const FVector& InWorldLocation)
{
    // z축이 없는 월드좌표
    FVector2D WorldLocation2D = FVector2D(InWorldLocation.X, InWorldLocation.Y);
    // 월드좌표 -> 미니맵 좌표 변환
    FVector2D MinimapCoordinates = ConvertWorldToMinimapCoordinates(WorldLocation2D);

    // UI에 미니맵 좌표를 알림
    if (OnPlayerMinimapCoordinateUpdated.IsBound())
    {
        OnPlayerMinimapCoordinateUpdated.Broadcast(MinimapCoordinates);
    }
}

FVector2D UMinimapViewModel::ConvertWorldToMinimapCoordinates(const FVector2D& InWorldLocation2D) const
{
    if(WorldRenderPoint == FVector2D::ZeroVector || WorldRenderLength == 0 || MinimapSize == FVector2D::ZeroVector)
    {
        UE_LOG(LogTemp, Warning, TEXT("MinimapViewModel::ConvertWorldToMinimapCoordinates - WorldMinPoint, WorldMaxPoint, or MinimapSize is not set properly."));
        return FVector2D();
    }

    FVector2D MinimapCoord;

    // 월드 렌더 영역의 최소점 계산(미니맵의 0,0)
    FVector2D WorldMinPoint = FVector2D(WorldRenderPoint.X - WorldRenderLength / 2.0f, WorldRenderPoint.Y - WorldRenderLength / 2.0f);

    // 월드 렌더 영역 내에서의 입력 좌표의 상대적인 위치 계산
    FVector2D RelativePosition = InWorldLocation2D - WorldMinPoint;

    // 입력 좌표가 월드 렌더 영역 밖에 있는지 검사
    if(RelativePosition.X < 0 || RelativePosition.Y < 0 || 
       RelativePosition.X > WorldRenderLength || RelativePosition.Y > WorldRenderLength)
    {
        UE_LOG(LogTemp, Warning, TEXT("MinimapViewModel::ConvertWorldToMinimapCoordinates - InWorldLocation2D is out of WorldRender area."));
        return FVector2D();
    }

    // 스케일링 팩터 계산(미니맵은 정사각형)
    float SquaredMinmapLength = MinimapSize.X;
    float ScaleFactor = SquaredMinmapLength / WorldRenderLength;

    // 미니맵 좌표 계산
    MinimapCoord.X = RelativePosition.X * ScaleFactor;
    MinimapCoord.Y = RelativePosition.Y * ScaleFactor;

    return FVector2D(MinimapCoord);
}
