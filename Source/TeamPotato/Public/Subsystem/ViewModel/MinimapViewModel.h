// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MinimapViewModel.generated.h"

// --- 신캡처2D에 미니맵 캡처 요청을 알림 ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMinimapCaptureRequested, FVector2D, InMinPoint, FVector2D, InMaxPoint);

// --- 플레이어의 미니맵 좌표를 델리게이트로 알림 ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerMinimapCoordinateUpdated, FVector2D, InMinimapCoordinates);

/**
 * 
 */
UCLASS(Blueprintable)
class TEAMPOTATO_API UMinimapViewModel : public UObject
{
	GENERATED_BODY()
	
public:
    ///
    /// Call by Model
    /// 
    // --- 미니맵 캡처 요청 함수 ---
    UFUNCTION(BlueprintCallable)
    void RequestMinimapCapture(FVector2D InMinPoint, FVector2D InMaxPoint);

    // --- 미니맵 플레이어 위치 변경 함수 ---
    void RequestPlayerMinimapCoordinateUpdate(const FVector& InWorldLocation);

    // --- 미니맵 사이즈 Setter(MinimapSceneCapture2D에서 강제 주입) ---
    void SetMinimapSize(const FVector2D& InMinimapSize) { MinimapSize = InMinimapSize; }
    void SetWorldRenderPoint(const FVector2D& InWorldRenderPoint) { WorldRenderPoint = InWorldRenderPoint; }
    void SetWorldRenderSize(const float& InWorldRenderSize) { WorldRenderLength = InWorldRenderSize; }

private:
    // --- 월드 좌표가 들어오면 미니맵 좌표로 바꾸는 함수 ---
    FVector2D ConvertWorldToMinimapCoordinates(const FVector2D& InWorldLocation2D) const;

public:
    // --- 미니맵 캡처 요청 델리게이트 ---
    UPROPERTY(BlueprintAssignable)
    FOnMinimapCaptureRequested OnMinimapCaptureRequested;

    // --- 플레이어의 미니맵 좌표를 델리게이트로 알림 ---
    UPROPERTY(BlueprintAssignable)
    FOnPlayerMinimapCoordinateUpdated OnPlayerMinimapCoordinateUpdated;

protected:
    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> MinimapMaterial = nullptr;

private:
    // --- 씬 캡처가 촬용한 월드 영역 ---
    // --- 월드 촬영 지점 ---
    FVector2D WorldRenderPoint = FVector2D::ZeroVector;
    // --- 월드 촬영 한변의 길이 ---
    float WorldRenderLength = 0;

    // --- 씬 캡처가 그린 미니맵 크기 ---
    FVector2D MinimapSize = FVector2D::ZeroVector;
};
