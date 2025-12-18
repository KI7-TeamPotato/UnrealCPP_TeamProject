// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ViewModel/MinimapViewModel.h"
#include "UI/Minimap/MinimapManager.h"

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
    
    // 월드좌표 -> 미니맵 UV좌표 변환
    if (MinimapManager)
    {
        FVector2D MinimapUV = MinimapManager->WorldToMinimapUV(WorldLocation2D);

        // UI에 미니맵 좌표를 알림
        if (OnPlayerMinimapCoordinateUpdated.IsBound())
        {
            OnPlayerMinimapCoordinateUpdated.Broadcast(MinimapUV);
        }
    }
}
