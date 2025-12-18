// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Minimap/MinimapSceneCapture2D.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Subsystem/MVVMSubsystem.h"
#include "Subsystem/ViewModel/MinimapViewModel.h"

AMinimapSceneCapture2D::AMinimapSceneCapture2D()
{
    InitializeCaptureComponent();
}

void AMinimapSceneCapture2D::BeginPlay()
{
    Super::BeginPlay();

    InitializeRenderTarget();

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

void AMinimapSceneCapture2D::OnMinimapCapture(FVector2D InMinPoint, FVector2D InMaxPoint)
{
    // 던전 촬영 위치 설정
    FVector DungeonCenter = FVector((InMinPoint.X + InMaxPoint.X) / 2.0f, (InMinPoint.Y + InMaxPoint.Y) / 2.0f, 0.0f);
    WorldCapturePoint = FVector2D(DungeonCenter.X, DungeonCenter.Y);
    SetActorLocation(FVector(WorldCapturePoint, 0.0f));
    
    // 던전 촬영 크기 설정
    FVector DungeonSize = FVector(InMaxPoint.X - InMinPoint.X, InMaxPoint.Y - InMinPoint.Y, 0.0f);
    WorldCaptureLength = FMath::Max(DungeonSize.X, DungeonSize.Y) * BorderRatio; // 약간 여유 공간 추가
    CaptureComp->OrthoWidth = WorldCaptureLength;

    // 씬 캡처 실행
    CaptureComp->CaptureScene();

    // 캡처 후에 그 정보를 뷰모델에 설정
    MinimapViewModel->SetMinimapSize(RenderTargetSize);
    MinimapViewModel->SetWorldRenderPoint(WorldCapturePoint);
    MinimapViewModel->SetWorldRenderSize(WorldCaptureLength);
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
