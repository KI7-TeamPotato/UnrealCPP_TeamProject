#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "Animation/Notify/AnimNotifyState_AttackEnable.h"
#include "Animation/Notify/AnimNotifyState_CancelInput.h"
#include "Combat/CombatAbilitySystemComponent.h"
#include "Combat/CombatTags.h"
#include "Component/PlayerResource.h"
#include "Component/WeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Data/WeaponDataAsset.h"
#include "Engine/Engine.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/SkeletalMeshActor.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Item/Weapon/SwordWeaponActor.h"
#include "Player/PlayerAnimation.h"
#include "Player/TestCharacter.h"
#include "UObject/UnrealType.h"

namespace
{
    constexpr float StepTime = 1.0f / 120.0f;

    void SetObject(UObject* Object, const TCHAR* Name, UObject* Value)
    {
        FindFProperty<FObjectPropertyBase>(Object->GetClass(), Name)->SetObjectPropertyValue_InContainer(Object, Value);
    }

    void AttackInput(ATestCharacter* Player)
    {
        struct { bool bIsAutoFiring = false; } Params;
        Player->ProcessEvent(Player->FindFunctionChecked(TEXT("OnAttack")), &Params);
    }

    void MoveInput(ATestCharacter* Player)
    {
        struct { FInputActionValue InValue = FInputActionValue(FVector2D(1.0f, 0.0f)); } Params;
        Player->ProcessEvent(Player->FindFunctionChecked(TEXT("OnMovementInput")), &Params);
    }

    struct FPlayerMeleeWorld
    {
        UWorld* World;
        uint64 InitialFrame = GFrameCounter;
        ATestCharacter* Player = nullptr;
        ASwordWeaponActor* Sword = nullptr;

        FPlayerMeleeWorld()
        {
            World = UWorld::CreateWorld(EWorldType::GamePreview, false);
            GEngine->CreateNewWorldContext(EWorldType::GamePreview).SetCurrentWorld(World);
            World->InitializeActorsForPlay(FURL());
        }

        ~FPlayerMeleeWorld()
        {
            World->DestroyWorld(false);
            GEngine->DestroyWorldContext(World);
            GFrameCounter = InitialFrame;
        }

        bool Spawn(const TCHAR* Name = TEXT("BP_PlayerKnight"))
        {
            const FString ClassPath = FString::Printf(TEXT("/Game/BluePrints/Test/Player/%s.%s_C"), Name, Name);
            UClass* Class = LoadClass<ATestCharacter>(nullptr, *ClassPath);
            if (!Class) return false;
            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            // Exercise real character/ABP/montage assets without starting menu/UI/game-instance subsystems.
            Player = World->SpawnActor<ATestCharacter>(Class, FVector(0, 0, 100), FRotator::ZeroRotator, Params);
            if (!Player || !Player->GetMesh()->GetAnimInstance()) return false;
            World->SpawnActor<APlayerController>()->Possess(Player);
            Player->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
            Player->GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
            Player->GetMesh()->bEnableUpdateRateOptimizations = false;

            Sword = World->SpawnActor<ASwordWeaponActor>(FVector(0, 0, 1000), FRotator::ZeroRotator, Params);
            auto* Data = NewObject<UWeaponDataAsset>(Sword);
            Data->WeaponType = EWeaponType::Sword;
            Data->AttackCost = 7.0f;
            Data->AttackSpeed = 1.0f;
            Data->AttackDamage = 10.0f;
            Sword->InitializeFromData(Data);
            Sword->SetOwner(Player);
            Sword->SetInstigator(Player);
            auto* Weapons = Player->GetWeaponComponent();
            SetObject(Weapons, TEXT("Owner"), Player);
            SetObject(Weapons, TEXT("ActivatedWeapon"), Sword);
            SetObject(Weapons, TEXT("CurrentWeapon"), Sword);
            Sword->SetOwnerComponent(Weapons);
            SetObject(Player->FindComponentByClass<UPlayerAnimation>(), TEXT("MainPlayer"), Player);
            Player->SetPlayerActivatedWeapon(EWeaponType::Sword);
            Player->GetResource()->SetMaxEnergy(100.0f);
            return true;
        }

        TArray<UAnimMontage*> Montages() const
        {
            TArray<UAnimMontage*> Result;
            for (const TCHAR* Name : {TEXT("AttackMontage_Sword_Combo1"), TEXT("AttackMontage_Sword_Combo2"), TEXT("AttackMontage_Sword_Combo3")})
                Result.Add(Cast<UAnimMontage>(FindFProperty<FObjectPropertyBase>(Player->GetClass(), Name)->GetObjectPropertyValue_InContainer(Player)));
            return Result;
        }

        bool Attacking() const
        {
            for (UAnimMontage* Montage : Montages())
                if (Montage && Player->IsCurrentSwordAttack(Montage)) return true;
            return false;
        }

        void Step(float DeltaTime = StepTime)
        {
            ++GFrameCounter;
            auto* Mesh = Player->GetMesh();
            Mesh->TickAnimation(DeltaTime, false);
            Mesh->RefreshBoneTransforms();
            Mesh->ConditionallyDispatchQueuedAnimEvents();
            Player->GetCharacterMovement()->TickComponent(DeltaTime, LEVELTICK_All, nullptr);
        }

        void Advance(float Seconds)
        {
            for (int32 Index = 0; Index < FMath::CeilToInt(Seconds / StepTime); ++Index) Step();
        }

        bool ReachInputWindow()
        {
            for (int32 Index = 0; Index < 300 && Attacking(); ++Index)
            {
                if (Player->bIsComboInputAvailable) return true;
                Step();
            }
            return false;
        }

        UCombatAbilitySystemComponent* SpawnTarget()
        {
            AActor* Actor = World->SpawnActor<AActor>();
            auto* ASC = NewObject<UCombatAbilitySystemComponent>(Actor);
            Actor->AddInstanceComponent(ASC);
            ASC->RegisterComponent();
            ASC->InitializeCombat(100.0f, 0.0f);
            return ASC;
        }
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerMeleeAssetsTest, "TeamPotato.Combat.PlayerMelee.AssetPlaybackAndRootLock",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPlayerMeleeAssetsTest::RunTest(const FString& Parameters)
{
    TSet<UAnimSequence*> Evaluated;
    const TArray<FString> Expected = {TEXT("Combo_Attack_01_01_Seq_InPlace"), TEXT("Combo_Attack_01_02_Seq_InPlace"), TEXT("Combo_Attack_02_01_Seq_InPlace")};
    for (const TCHAR* Name : {TEXT("BP_TestCharacter"), TEXT("BP_PlayerKnight"), TEXT("BP_PlayerGunner"), TEXT("BP_PlayerEngne")})
    {
        FPlayerMeleeWorld Fixture;
        if (!TestTrue(FString(Name) + TEXT(" spawns with its real ABP"), Fixture.Spawn(Name))) continue;
        FPlayerMeleeWorld IdleFixture;
        if (!TestTrue(FString(Name) + TEXT(" idle comparison actor spawns"), IdleFixture.Spawn(Name))) continue;
        Fixture.Advance(0.15f);
        IdleFixture.Advance(0.15f);
        const TArray<UAnimMontage*> Montages = Fixture.Montages();
        for (int32 Index = 0; Index < Montages.Num(); ++Index)
        {
            UAnimMontage* Montage = Montages[Index];
            if (!TestNotNull(FString(Name) + TEXT(" configures all three attacks"), Montage)) continue;
            if (!TestTrue(TEXT("One full-body DefaultSlot animation segment per hit"), Montage->SlotAnimTracks.Num() == 1 &&
                Montage->SlotAnimTracks[0].SlotName == TEXT("DefaultSlot") && Montage->SlotAnimTracks[0].AnimTrack.AnimSegments.Num() == 1)) continue;
            UAnimSequence* Sequence = Cast<UAnimSequence>(Montage->SlotAnimTracks[0].AnimTrack.AnimSegments[0].GetAnimReference());
            if (!TestNotNull(TEXT("The montage contains an animation sequence"), Sequence)) continue;
            TestEqual(TEXT("Requested attack order"), Sequence->GetName(), Expected[Index]);
            TestTrue(TEXT("Attack sequence locks its root"), Sequence->bForceRootLock);
            TestFalse(TEXT("Attack cannot drive character root motion"), Montage->HasRootMotion());
            int32 HitWindows = 0, ComboWindows = 0;
            for (const FAnimNotifyEvent& Event : Montage->Notifies)
            {
                if (Cast<UAnimNotifyState_AttackEnable>(Event.NotifyStateClass)) ++HitWindows;
                if (Cast<UAnimNotifyState_CancelInput>(Event.NotifyStateClass)) ++ComboWindows;
            }
            TestEqual(TEXT("Exactly one melee hit window"), HitWindows, 1);
            TestEqual(TEXT("Only the first two hits accept continuation"), ComboWindows, Index < 2 ? 1 : 0);
            // Use the gameplay entry point so any ABP attack-state conditions also participate.
            if (Index == 0) Fixture.Player->PlaySwordAttackMontage();
            else
            {
                Fixture.Player->bIsOnComboInput = true;
                Fixture.Player->PlaySwordAttackMontage_Combo();
            }
            TestTrue(FString(Name) + TEXT(" can play this attack with its own animation instance"), Fixture.Player->IsCurrentSwordAttack(Montage));
            auto* ActualMesh = Fixture.Player->GetMesh();
            auto* IdleMesh = IdleFixture.Player->GetMesh();
            TestTrue(TEXT("Actual player has right-hand and upper-arm bones"), ActualMesh->GetBoneIndex(TEXT("hand_r")) != INDEX_NONE && ActualMesh->GetBoneIndex(TEXT("upperarm_r")) != INDEX_NONE);
            double LargestHandDifference = 0.0, LargestUpperArmDifference = 0.0;
            for (int32 Frame = 0; Frame < 60; ++Frame)
            {
                Fixture.Step();
                IdleFixture.Step();
                const FTransform ActualHand = ActualMesh->GetSocketTransform(TEXT("hand_r"), RTS_Component);
                const FTransform IdleHand = IdleMesh->GetSocketTransform(TEXT("hand_r"), RTS_Component);
                const FQuat ActualArm = ActualMesh->GetSocketTransform(TEXT("upperarm_r"), RTS_Component).GetRotation();
                const FQuat IdleArm = IdleMesh->GetSocketTransform(TEXT("upperarm_r"), RTS_Component).GetRotation();
                LargestHandDifference = FMath::Max(LargestHandDifference, FVector::Distance(ActualHand.GetTranslation(), IdleHand.GetTranslation()));
                LargestUpperArmDifference = FMath::Max(LargestUpperArmDifference, ActualArm.AngularDistance(IdleArm));
            }
            TestTrue(TEXT("Actual montage playback advances"), Fixture.Player->GetMesh()->GetAnimInstance()->Montage_GetPosition(Montage) > 0.1f);
            AddInfo(FString::Printf(TEXT("%s hit %d evaluated pose: hand delta=%.2f cm, upper-arm delta=%.3f rad"), Name, Index + 1, LargestHandDifference, LargestUpperArmDifference));
            TestTrue(FString(Name) + TEXT(" attack visibly changes the evaluated hand and upper-arm pose from idle"), LargestHandDifference > 5.0 && LargestUpperArmDifference > 0.05);

            if (!Evaluated.Contains(Sequence))
            {
                Evaluated.Add(Sequence);
                auto* Preview = Fixture.World->SpawnActor<ASkeletalMeshActor>();
                auto* Mesh = Preview->GetSkeletalMeshComponent();
                Mesh->SetSkeletalMesh(LoadObject<USkeletalMesh>(nullptr, TEXT("/Game/Assets/Sword_and_Shield/Demo/Mannequin_UE4/Meshes/SK_Mannequin.SK_Mannequin")));
                Mesh->PlayAnimation(Sequence, false);
                FTransform InitialRoot;
                float LargestTranslation = 0.0f, LargestRotation = 0.0f;
                FVector InitialHand;
                float LargestHandExcursion = 0.0f;
                for (int32 Sample = 0; Sample <= 35; ++Sample)
                {
                    Mesh->SetPosition(Sequence->GetPlayLength() * Sample / 35.0f, false);
                    Mesh->TickAnimation(0.0f, false);
                    Mesh->RefreshBoneTransforms();
                    const FTransform Root = Mesh->GetComponentSpaceTransforms()[0];
                    const FVector Hand = Mesh->GetSocketTransform(TEXT("hand_r"), RTS_Component).GetTranslation();
                    if (Sample == 0)
                    {
                        InitialRoot = Root;
                        InitialHand = Hand;
                    }
                    LargestTranslation = FMath::Max(LargestTranslation, float(FVector::Distance(Root.GetTranslation(), InitialRoot.GetTranslation())));
                    LargestRotation = FMath::Max(LargestRotation, float(Root.GetRotation().AngularDistance(InitialRoot.GetRotation())));
                    LargestHandExcursion = FMath::Max(LargestHandExcursion, float(FVector::Distance(Hand, InitialHand)));
                }
                TestTrue(Sequence->GetName() + TEXT(" root stays fixed throughout the evaluated pose"), LargestTranslation < 0.01f && LargestRotation < 0.001f);
                TestTrue(Sequence->GetName() + TEXT(" evaluated pose moves its sword hand, not just the playback clock"), LargestHandExcursion > 10.0f);
                Preview->Destroy();
            }
        }
        Fixture.Player->StopAnimMontage();
        Fixture.Advance(0.3f);
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerMeleeFullBodyPoseTest, "TeamPotato.Combat.PlayerMelee.FullBodyMatchesSourcePose",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPlayerMeleeFullBodyPoseTest::RunTest(const FString& Parameters)
{
    const TArray<FName> Bones = {TEXT("pelvis"), TEXT("hand_r"), TEXT("foot_l"), TEXT("foot_r")};
    for (const TCHAR* Name : {TEXT("BP_TestCharacter"), TEXT("BP_PlayerKnight"), TEXT("BP_PlayerGunner"), TEXT("BP_PlayerEngne")})
    {
        FPlayerMeleeWorld Fixture;
        if (!TestTrue(FString(Name) + TEXT(" loads its real animation blueprint"), Fixture.Spawn(Name))) continue;
        Fixture.Advance(0.15f);
        auto* ActualMesh = Fixture.Player->GetMesh();
        auto* Preview = Fixture.World->SpawnActor<ASkeletalMeshActor>();
        auto* ReferenceMesh = Preview->GetSkeletalMeshComponent();
        // Evaluate the requested sequence on the same target mesh, including identical skeleton remapping.
        ReferenceMesh->SetSkeletalMesh(ActualMesh->GetSkeletalMeshAsset());
        ReferenceMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
        ReferenceMesh->bEnableUpdateRateOptimizations = false;
        const TArray<UAnimMontage*> Montages = Fixture.Montages();
        for (int32 Hit = 0; Hit < Montages.Num(); ++Hit)
        {
            UAnimMontage* Montage = Montages[Hit];
            if (!TestTrue(TEXT("Each attack has one sequence to compare"), Montage && Montage->SlotAnimTracks.Num() == 1 &&
                Montage->SlotAnimTracks[0].AnimTrack.AnimSegments.Num() == 1)) continue;
            const FAnimSegment& Segment = Montage->SlotAnimTracks[0].AnimTrack.AnimSegments[0];
            auto* Sequence = Cast<UAnimSequence>(Segment.GetAnimReference());
            if (!TestNotNull(TEXT("Attack sequence exists"), Sequence)) continue;
            ReferenceMesh->PlayAnimation(Sequence, false);
            if (Hit == 0) Fixture.Player->PlaySwordAttackMontage();
            else
            {
                Fixture.Player->bIsOnComboInput = true;
                Fixture.Player->PlaySwordAttackMontage_Combo();
            }
            TestTrue(TEXT("Gameplay starts the expected hit"), Fixture.Player->IsCurrentSwordAttack(Montage));
            TArray<double> MaxDistance, MaxAngle;
            MaxDistance.Init(0.0, Bones.Num());
            MaxAngle.Init(0.0, Bones.Num());
            int32 ComparedFrames = 0;
            for (int32 Frame = 0; Frame < 54; ++Frame)
            {
                Fixture.Step();
                const float Position = ActualMesh->GetAnimInstance()->Montage_GetPosition(Montage);
                // Ignore the intentional blend from idle/the previous attack; sample the settled swing.
                if (Position < 0.25f || Position > 0.45f) continue;
                ReferenceMesh->SetPosition(Segment.ConvertTrackPosToAnimPos(Position), false);
                ReferenceMesh->TickAnimation(0.0f, false);
                ReferenceMesh->RefreshBoneTransforms();
                ++ComparedFrames;
                for (int32 Bone = 0; Bone < Bones.Num(); ++Bone)
                {
                    const FTransform Actual = ActualMesh->GetSocketTransform(Bones[Bone], RTS_Component);
                    const FTransform Expected = ReferenceMesh->GetSocketTransform(Bones[Bone], RTS_Component);
                    MaxDistance[Bone] = FMath::Max(MaxDistance[Bone], FVector::Distance(Actual.GetTranslation(), Expected.GetTranslation()));
                    MaxAngle[Bone] = FMath::Max(MaxAngle[Bone], Actual.GetRotation().AngularDistance(Expected.GetRotation()));
                }
            }
            TestTrue(TEXT("Compares multiple evaluated frames after montage blend-in"), ComparedFrames >= 20);
            for (int32 Bone = 0; Bone < Bones.Num(); ++Bone)
            {
                const FString Label = FString::Printf(TEXT("%s hit %d %s"), Name, Hit + 1, *Bones[Bone].ToString());
                TestTrue(Label + TEXT(" exists on the target mesh"), ActualMesh->GetBoneIndex(Bones[Bone]) != INDEX_NONE);
                AddInfo(FString::Printf(TEXT("%s source-pose error: %.4f cm, %.6f rad"), *Label, MaxDistance[Bone], MaxAngle[Bone]));
                TestTrue(Label + TEXT(" preserves the requested sequence's full-body motion"), MaxDistance[Bone] <= 1.0 && MaxAngle[Bone] <= 0.01);
            }
        }
        Fixture.Player->StopAnimMontage();
        Fixture.Advance(0.3f);
        Preview->Destroy();
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerMeleeThreeHitTest, "TeamPotato.Combat.PlayerMelee.ThreeHitsDamageAndStationaryMovement",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPlayerMeleeThreeHitTest::RunTest(const FString& Parameters)
{
    FPlayerMeleeWorld Fixture;
    if (!TestTrue(TEXT("Knight fixture loads"), Fixture.Spawn())) return false;
    ATestCharacter* Player = Fixture.Player;
    const TArray<UAnimMontage*> Montages = Fixture.Montages();
    if (!TestTrue(TEXT("Three montage defaults configured"), !Montages.Contains(nullptr))) return false;
    UCombatAbilitySystemComponent* Target = Fixture.SpawnTarget();
    UCapsuleComponent* Hitbox = Fixture.Sword->FindComponentByClass<UCapsuleComponent>();
    const FVector Start = Player->GetActorLocation();
    Player->GetCharacterMovement()->Velocity = FVector(300, 0, 0);
    Player->AddMovementInput(FVector::ForwardVector, 1.0f, true);
    AttackInput(Player);
    TestTrue(TEXT("Attack immediately clears movement velocity and queued movement"), Player->GetVelocity().IsNearlyZero() && Player->GetPendingMovementInputVector().IsNearlyZero());
    TArray<UAnimMontage*> Seen;
    TSet<UAnimMontage*> Queued;
    TSet<UAnimMontage*> Damaged;
    float MaxDisplacement = 0.0f;
    bool bBlockedEveryInput = true, bCheckedThirdContinuation = false;
    for (int32 Frame = 0; Frame < 600 && Fixture.Attacking(); ++Frame)
    {
        UAnimMontage* Current = Player->GetCurrentMontage();
        if (Current && !Seen.Contains(Current))
        {
            Seen.Add(Current);
            TestEqual(TEXT("Each swing consumes its own energy cost"), Player->GetResource()->GetEnergyAmount(), 100.0f - 7.0f * Seen.Num());
        }
        if (Current && Player->bIsComboInputAvailable && Current != Montages[2] && !Queued.Contains(Current))
        {
            AttackInput(Player);
            TestTrue(TEXT("Attack input is buffered during the real notify window"), Player->bIsOnComboInput);
            Queued.Add(Current);
        }
        if (Current == Montages[2] && !bCheckedThirdContinuation)
        {
            Player->bIsOnComboInput = true;
            Player->PlaySwordAttackMontage_Combo();
            TestTrue(TEXT("Third hit cannot restart itself or spend another cost"), Player->GetCurrentMontage() == Montages[2] && Player->GetResource()->GetEnergyAmount() == 79.0f);
            bCheckedThirdContinuation = true;
        }
        if (Current && Hitbox->GetCollisionEnabled() == ECollisionEnabled::QueryOnly && !Damaged.Contains(Current))
        {
            const float Before = Target->GetHealth();
            Fixture.Sword->DamageToTarget(Target->GetOwner());
            Fixture.Sword->DamageToTarget(Target->GetOwner());
            TestEqual(TEXT("Each real notify window hits the same target only once"), Target->GetHealth(), Before - 10.0f);
            Damaged.Add(Current);
        }
        MoveInput(Player);
        bBlockedEveryInput &= Player->GetPendingMovementInputVector().IsNearlyZero();
        Fixture.Step();
        MaxDisplacement = FMath::Max(MaxDisplacement, float(FVector::Distance(Start, Player->GetActorLocation())));
    }
    TestTrue(TEXT("Real notify execution plays exactly hit 1 then 2 then 3"), Seen == Montages);
    TestEqual(TEXT("All three hit windows execute"), Damaged.Num(), 3);
    TestEqual(TEXT("Total damage from the complete combo"), Target->GetHealth(), 70.0f);
    TestTrue(TEXT("Movement input stays blocked through attack blends"), bBlockedEveryInput);
    TestTrue(TEXT("The character stays in place for the complete combo"), MaxDisplacement < 0.1f);
    TestFalse(TEXT("Combo completes"), Fixture.Attacking());
    TestTrue(TEXT("Completion closes hitbox and clears combo input"), Hitbox->GetCollisionEnabled() == ECollisionEnabled::NoCollision && !Player->bIsComboInputAvailable && !Player->bIsOnComboInput);
    MoveInput(Player);
    TestFalse(TEXT("Movement input resumes after the combo"), Player->GetPendingMovementInputVector().IsNearlyZero());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerMeleeInterruptTest, "TeamPotato.Combat.PlayerMelee.InterruptionEnergyAndAttackBlock",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPlayerMeleeInterruptTest::RunTest(const FString& Parameters)
{
    FPlayerMeleeWorld Fixture;
    if (!TestTrue(TEXT("Knight fixture loads"), Fixture.Spawn())) return false;
    ATestCharacter* Player = Fixture.Player;
    const TArray<UAnimMontage*> Montages = Fixture.Montages();
    if (!TestTrue(TEXT("Three montage defaults configured"), !Montages.Contains(nullptr))) return false;
    AttackInput(Player);
    if (!TestTrue(TEXT("First combo input window opens"), Fixture.ReachInputWindow())) return false;
    AttackInput(Player);
    Player->StopAnimMontage(Montages[0]);
    MoveInput(Player);
    TestTrue(TEXT("Movement remains blocked while interrupted attack blends out"), Player->GetPendingMovementInputVector().IsNearlyZero());
    Fixture.Advance(0.4f);
    TestFalse(TEXT("Cancelled window does not launch the buffered second hit"), Fixture.Attacking());
    TestEqual(TEXT("Interrupted continuation costs no additional energy"), Player->GetResource()->GetEnergyAmount(), 93.0f);
    TestTrue(TEXT("Interrupt clears both buffer flags"), !Player->bIsOnComboInput && !Player->bIsComboInputAvailable);

    Player->GetResource()->SetMaxEnergy(7.0f);
    AttackInput(Player);
    if (!TestTrue(TEXT("New attack after interruption opens its window"), Fixture.ReachInputWindow())) return false;
    AttackInput(Player);
    Fixture.Advance(1.5f);
    TestFalse(TEXT("Insufficient energy cannot start the second hit"), Fixture.Attacking());
    TestEqual(TEXT("Energy never becomes negative"), Player->GetResource()->GetEnergyAmount(), 0.0f);
    TestTrue(TEXT("Failed continuation clears both buffer flags"), !Player->bIsOnComboInput && !Player->bIsComboInputAvailable);

    Player->GetResource()->SetMaxEnergy(100.0f);
    AttackInput(Player);
    for (int32 Hit = 0; Hit < 2; ++Hit)
    {
        if (!TestTrue(TEXT("Combo window opens before attack block"), Fixture.ReachInputWindow())) return false;
        Player->bIsOnComboInput = true;
        Player->PlaySwordAttackMontage_Combo();
    }
    TestTrue(TEXT("Third hit begins before GAS interruption"), Player->IsCurrentSwordAttack(Montages[2]));
    Fixture.Sword->BeginAttack();
    auto* ASC = CastChecked<UCombatAbilitySystemComponent>(Player->GetAbilitySystemComponent());
    ASC->AddLooseGameplayTag(CombatTags::State_AttackBlocked);
    TestFalse(TEXT("GAS attack block immediately stops third-hit playback"), Player->GetMesh()->GetAnimInstance()->Montage_IsPlaying(Montages[2]));
    TestTrue(TEXT("GAS attack block immediately closes melee collision"), Fixture.Sword->FindComponentByClass<UCapsuleComponent>()->GetCollisionEnabled() == ECollisionEnabled::NoCollision);
    Fixture.Advance(0.4f);
    AttackInput(Player);
    TestFalse(TEXT("Blocked player cannot restart a combo"), Fixture.Attacking());
    ASC->RemoveLooseGameplayTag(CombatTags::State_AttackBlocked);
    AttackInput(Player);
    TestTrue(TEXT("Attack resumes at hit one after the block expires"), Player->IsCurrentSwordAttack(Montages[0]));
    return true;
}

#endif
