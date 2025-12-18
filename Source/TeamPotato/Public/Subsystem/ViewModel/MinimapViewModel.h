// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MinimapViewModel.generated.h"

// --- 신캡처2D에 미니맵 캡처 요청을 알림 ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMinimapCaptureRequested, FVector2D, InMinPoint, FVector2D, InMaxPoint);

// --- 플레이어의 미니맵 좌표를 델리게이트로 알림 ---
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerMinimapCoordinateUpdated, FVector2D, InMinimapCoordinates);

class UMinimapManager;
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

public:
    // --- 미니맵 캡처 요청 델리게이트 ---
    UPROPERTY(BlueprintAssignable)
    FOnMinimapCaptureRequested OnMinimapCaptureRequested;

    // --- 플레이어의 미니맵 좌표를 델리게이트로 알림 ---
    UPROPERTY(BlueprintAssignable)
    FOnPlayerMinimapCoordinateUpdated OnPlayerMinimapCoordinateUpdated;

private:
    TObjectPtr<UMinimapManager> MinimapManager = nullptr;
};
