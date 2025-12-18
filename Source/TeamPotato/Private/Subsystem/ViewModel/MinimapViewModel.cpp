// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ViewModel/MinimapViewModel.h"
#include "UI/Minimap/MinimapManager.h"
#include "Materials/MaterialInstanceDynamic.h"

// --- 맵 생성시에 미니맵 캡처 요청 함수 ---
void UMinimapViewModel::RequestMinimapCapture(FVector2D InMinPoint, FVector2D InMaxPoint)
{
    if (OnMinimapCaptureRequested.IsBound())
    {
        OnMinimapCaptureRequested.Broadcast(InMinPoint, InMaxPoint);
    }
}

void UMinimapViewModel::UpdatePlayerPosition(const FVector& InWorldLocation, float InYaw)
{
    if (MinimapManager)
    {
        MinimapManager->UpdatePlayerPosition(InWorldLocation, InYaw);
    }
}

void UMinimapViewModel::SetMinimapManager(UMinimapManager* InMinimapManager)
{
    MinimapManager = InMinimapManager;

    // 초기화 완료 알림
    if (OnMinimapInitialized.IsBound())
    {
        OnMinimapInitialized.Broadcast();
    }
}

UMaterialInstanceDynamic* UMinimapViewModel::GetMinimapMaterial() const
{
    if (MinimapManager)
    {
        return MinimapManager->GetMinimapMaterial();
    }
    return nullptr;
}