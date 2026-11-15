// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/LastBoss.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/TimelineComponent.h"
#include "Engine/World.h"
#include "Engine/LatentActionManager.h"
#include "TeamPotato/Logic/DungeonGanarator.h"
#include "TeamPotato/Room/RoomBase.h"
#include "Kismet/GameplayStatics.h"
ALastBoss::ALastBoss()
{
    MaxHealth = 1200;
}

void ALastBoss::BeginPlay()
{
    Super::BeginPlay();
}

void ALastBoss::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (ActivePattern != 0 || !PatternHazards.IsEmpty()) StopPattern(true);
    Super::EndPlay(EndPlayReason);
}

void ALastBoss::ExecutePattern(int32 PatternIndex)
{
    if (!HasAuthority() || bDeathHandled || IsActorBeingDestroyed() || ActivePattern != 0) return;
    ActivePattern = ++PatternSerial;
    const USkeletalMeshComponent* BossMesh = GetMesh();
    UAnimInstance* Anim = BossMesh ? BossMesh->GetAnimInstance() : nullptr;
    if (PatternIndex < 0 || PatternIndex > 4 || !BossMesh || !BossMesh->GetSkeletalMeshAsset() || !Anim)
    {
        FinishCurrentPattern();
        return;
    }
    const FAnimMontageInstance* Previous = Anim->GetActiveMontageInstance();
    PatternMontageInstance = Previous ? Previous->GetInstanceID() : INDEX_NONE;
    Super::ExecutePattern(PatternIndex);
}

void ALastBoss::TrackPatternMontage(UAnimMontage* Montage)
{
    if (ActivePattern == 0) return;
    UAnimInstance* Anim = GetMesh()->GetAnimInstance();
    FAnimMontageInstance* Instance = Anim && Montage ? Anim->GetActiveInstanceForMontage(Montage) : nullptr;
    if (!Instance || Instance->GetInstanceID() == PatternMontageInstance)
    {
        // Play Montage can fail before its Blueprint output delegates are bound.
        PatternMontageInstance = INDEX_NONE;
        CancelActivePattern();
        return;
    }

    PatternMontageInstance = Instance->GetInstanceID();
    const uint64 CastSerial = ActivePattern;
    const TWeakObjectPtr<ALastBoss> WeakBoss(this);
    // Keep the Blueprint proxy's notify/completion behavior and identify callbacks by cast.
    // An old montage may still be blending out when the next attack starts.
    FOnMontageBlendingOutStarted OriginalBlend = MoveTemp(Instance->OnMontageBlendingOutStarted);
    Instance->OnMontageBlendingOutStarted.BindLambda(
        [WeakBoss, CastSerial, OriginalBlend = MoveTemp(OriginalBlend)](UAnimMontage* Asset, bool bInterrupted)
        {
            OriginalBlend.ExecuteIfBound(Asset, bInterrupted);
            if (ALastBoss* Boss = WeakBoss.Get(); bInterrupted && Boss && Boss->ActivePattern == CastSerial)
                Boss->CancelActivePattern();
        });
    FOnMontageEnded OriginalEnd = MoveTemp(Instance->OnMontageEnded);
    Instance->OnMontageEnded.BindLambda(
        [WeakBoss, CastSerial, OriginalEnd = MoveTemp(OriginalEnd)](UAnimMontage* Asset, bool bInterrupted)
        {
            ALastBoss* Boss = WeakBoss.Get();
            // The proxy still unbinds its notifies. A cancelled cast must not start the next montage in a chain.
            OriginalEnd.ExecuteIfBound(Asset, bInterrupted || !Boss || Boss->ActivePattern != CastSerial);
            if (bInterrupted && Boss && Boss->ActivePattern == CastSerial) Boss->CancelActivePattern();
        });
}

void ALastBoss::TrackPatternHazard(AActor* Hazard)
{
    if (!IsValid(Hazard)) return;
    if (ActivePattern == 0)
    {
        Hazard->Destroy();
        return;
    }
    for (auto It = PatternHazards.CreateIterator(); It; ++It)
        if (!It.Key().IsValid()) It.RemoveCurrent();
    PatternHazards.Add(Hazard, ActivePattern);
}

void ALastBoss::FinishCurrentPattern()
{
    if (ActivePattern == 0) return;
    ActivePattern = 0;
    PatternMontageInstance = INDEX_NONE;
    Super::FinishCurrentPattern();
}

void ALastBoss::StopPattern(bool bEndEncounter)
{
    const uint64 StoppedPattern = ActivePattern;
    ActivePattern = 0;
    GetWorld()->GetLatentActionManager().RemoveActionsForObject(this);
    TInlineComponentArray<UTimelineComponent*> Timelines(this);
    for (UTimelineComponent* Timeline : Timelines) Timeline->Stop();
    EndBeamSweep();
    if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
    {
        if (FAnimMontageInstance* Instance = Anim->GetMontageInstanceForID(PatternMontageInstance))
            if (Instance->IsActive()) Anim->Montage_Stop(0.f, Instance->Montage);
    }
    PatternMontageInstance = INDEX_NONE;
    for (auto It = PatternHazards.CreateIterator(); It; ++It)
    {
        if (!It.Key().IsValid() || bEndEncounter || It.Value() == StoppedPattern)
        {
            if (AActor* Hazard = It.Key().Get()) Hazard->Destroy();
            It.RemoveCurrent();
        }
    }
}

void ALastBoss::CancelActivePattern()
{
    if (ActivePattern == 0) return;
    StopPattern(false);
    Super::FinishCurrentPattern();
}

void ALastBoss::OnDie()
{
    if (bDeathHandled || !HasAuthority()) return;
    const bool bHadActivePattern = ActivePattern != 0;
    StopPattern(true);
    Super::OnDie();
    if (bHadActivePattern) Super::FinishCurrentPattern();

    ARoomBase* Room = EncounterRoom.Get();
    auto* Dungeon = Room ? Cast<ADungeonGanarator>(Room->GetOwner()) : nullptr;
    if (Dungeon && Dungeon->HasNextChapter())
    {
        Dungeon->OpenNextChapterPortal(*Room);
        return;
    }

    constexpr float VictoryDelay = 14.0f;
    if (VictoryWidgetClass && GetLifeSpan() > 0.0f && GetLifeSpan() <= VictoryDelay)
        SetLifeSpan(VictoryDelay + 1.0f);
    GetWorldTimerManager().SetTimer(VictoryWidgetTimerHandle, this, &ALastBoss::ShowVictoryWidget, VictoryDelay, false);
}

float ALastBoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

}

void ALastBoss::ShowVictoryWidget()
{
    if (VictoryWidgetClass)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PC)
        {
            VictoryWidget = CreateWidget<UUserWidget>(PC, VictoryWidgetClass);
            if (VictoryWidget)
            {
                VictoryWidget->AddToViewport();

                // 마우스 커서 및 입력 모드 설정
                PC->SetShowMouseCursor(true);
                FInputModeUIOnly InputMode;
                InputMode.SetWidgetToFocus(VictoryWidget->TakeWidget());
                PC->SetInputMode(InputMode);
            }
        }
    }
}
