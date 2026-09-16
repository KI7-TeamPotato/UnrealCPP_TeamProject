// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Minimap/MinimapSceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Subsystem/MVVMSubsystem.h"
#include "Subsystem/ViewModel/MinimapViewModel.h"
#include "UI/Minimap/MinimapManager.h"
#include "Common/MyGameSettings.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

AMinimapSceneCapture2D::AMinimapSceneCapture2D()
{
    InitializeCaptureComponent();
}

void AMinimapSceneCapture2D::BeginPlay()
{
    Super::BeginPlay();

    InitializeRenderTarget();

    // 미니맵 매니저 초기화 (미니맵 텍스처, 월드 최소 좌표(기준), 던전 길이)
    if (!MinimapManager)
    {
        MinimapManager = NewObject<UMinimapManager>(this);
    }

    RequestMinimapAssets();

    // 뷰모델이 설정되어 있지 않다면 MVVM 서브시스템에서 가져와 설정
    if (!MinimapViewModel)
    {
        if (UMVVMSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UMVVMSubsystem>())
        {
            MinimapViewModel = Subsystem->GetMinimapViewModel();
            MinimapViewModel->OnMinimapCaptureRequested.AddDynamic(this, &AMinimapSceneCapture2D::OnMinimapCapture);
        }
    }

}

void AMinimapSceneCapture2D::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (MinimapAssetLoadHandle.IsValid())
    {
        MinimapAssetLoadHandle->CancelHandle();
        MinimapAssetLoadHandle.Reset();
    }

    if (MinimapViewModel)
    {
        MinimapViewModel->OnMinimapCaptureRequested.RemoveDynamic(this, &AMinimapSceneCapture2D::OnMinimapCapture);
    }

    Super::EndPlay(EndPlayReason);
}

void AMinimapSceneCapture2D::OnMinimapCapture(FVector2D InMinPoint, FVector2D InMaxPoint)
{
    if (!MinimapBaseMaterial || !MinimapPlayerIcon)
    {
        PendingMinPoint = InMinPoint;
        PendingMaxPoint = InMaxPoint;
        bHasPendingCapture = true;

        if (!MinimapAssetLoadHandle.IsValid())
        {
            RequestMinimapAssets();
        }
        return;
    }

    CaptureMinimap(InMinPoint, InMaxPoint);
}

void AMinimapSceneCapture2D::RequestMinimapAssets()
{
    const UMyGameSettings* GameSettings = UMyGameSettings::Get();
    if (!GameSettings)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot load minimap assets because game settings are unavailable."));
        return;
    }

    MinimapBaseMaterial = GameSettings->MinimapBaseMaterial.Get();
    MinimapPlayerIcon = GameSettings->MinimapPlayerIcon.Get();
    if (MinimapBaseMaterial && MinimapPlayerIcon)
    {
        HandleMinimapAssetsLoaded();
        return;
    }

    TArray<FSoftObjectPath> AssetPaths;
    if (!GameSettings->MinimapBaseMaterial.IsNull())
    {
        AssetPaths.AddUnique(GameSettings->MinimapBaseMaterial.ToSoftObjectPath());
    }
    if (!GameSettings->MinimapPlayerIcon.IsNull())
    {
        AssetPaths.AddUnique(GameSettings->MinimapPlayerIcon.ToSoftObjectPath());
    }

    if (AssetPaths.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Minimap material or player icon is not configured."));
        return;
    }

    MinimapAssetLoadHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
        AssetPaths,
        FStreamableDelegate::CreateUObject(this, &AMinimapSceneCapture2D::HandleMinimapAssetsLoaded));
}

void AMinimapSceneCapture2D::HandleMinimapAssetsLoaded()
{
    const UMyGameSettings* GameSettings = UMyGameSettings::Get();
    if (GameSettings)
    {
        MinimapBaseMaterial = GameSettings->MinimapBaseMaterial.Get();
        MinimapPlayerIcon = GameSettings->MinimapPlayerIcon.Get();
    }

    MinimapAssetLoadHandle.Reset();

    if (!MinimapBaseMaterial || !MinimapPlayerIcon)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load required minimap assets asynchronously."));
        return;
    }

    if (bHasPendingCapture)
    {
        bHasPendingCapture = false;
        CaptureMinimap(PendingMinPoint, PendingMaxPoint);
    }
}

void AMinimapSceneCapture2D::CaptureMinimap(FVector2D InMinPoint, FVector2D InMaxPoint)
{
    if (!MinimapManager || !CaptureComp || !CaptureComp->TextureTarget)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot capture minimap because required runtime objects are unavailable."));
        return;
    }

    // 던전 촬영 위치 설정
    WorldMinPoint = InMinPoint;
    WorldMaxPoint = InMaxPoint;
    FVector2D DungeonCenter = FVector2D(
        (InMinPoint.X + InMaxPoint.X) / 2.0f, 
        (InMinPoint.Y + InMaxPoint.Y) / 2.0f);
    SetActorLocation(FVector(DungeonCenter, GetActorLocation().Z));
    
    // 던전 촬영 크기 설정
    FVector2D DungeonSize = InMaxPoint - InMinPoint;
    CaptureOrthoWidth = FMath::Max(DungeonSize.X, DungeonSize.Y) * BorderRatio; // 약간 여유 공간 추가
    CaptureComp->OrthoWidth = CaptureOrthoWidth;

    // 씬 캡처 실행
    CaptureComp->CaptureScene();

    FVector2D AdjustedMinPoint = DungeonCenter - FVector2D(CaptureOrthoWidth / 2.0f, CaptureOrthoWidth / 2.0f);

    MinimapManager->InitializeMinimapManager(
        CaptureComp->TextureTarget,
        AdjustedMinPoint,
        CaptureOrthoWidth,
        MinimapBaseMaterial,
        MinimapPlayerIcon);

    // 뷰모델에 미니맵 매니저 설정
    if (MinimapViewModel)
    {
        MinimapViewModel->SetMinimapManager(MinimapManager);
    }
}

void AMinimapSceneCapture2D::InitializeCaptureComponent()
{
    // 매 프레임, 이동시에 캡처 비활성화
    CaptureComp = GetCaptureComponent2D();
    CaptureComp->bCaptureEveryFrame = false;
    CaptureComp->bCaptureOnMovement = false;

    // 직교 투영 설정, 
    CaptureComp->ProjectionType = ECameraProjectionMode::Orthographic;
    CaptureComp->CaptureSource = ESceneCaptureSource::SCS_BaseColor;
}

void AMinimapSceneCapture2D::InitializeRenderTarget()
{
    // 렌더 타깃의 사이즈 설정
    if (CaptureComp->TextureTarget)
    {
        CaptureComp->TextureTarget->SizeX = RenderTargetSize.X;
        CaptureComp->TextureTarget->SizeY = RenderTargetSize.Y;
    }
}
