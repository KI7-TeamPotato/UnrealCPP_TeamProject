#if WITH_DEV_AUTOMATION_TESTS && WITH_EDITOR

#include "Misc/AutomationTest.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"
#include "Animation/Skeleton.h"
#include "Component/WeaponComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Item/Weapon/GunWeaponActor.h"
#include "Player/TestCharacter.h"
#include "UObject/UnrealType.h"

namespace
{
    constexpr float GunnerPoseStep = 1.0f / 120.0f;

    void SetGunnerObject(UObject* Object, const TCHAR* PropertyName, UObject* Value)
    {
        FObjectPropertyBase* Property = FindFProperty<FObjectPropertyBase>(Object->GetClass(), PropertyName);
        check(Property);
        Property->SetObjectPropertyValue_InContainer(Object, Value);
    }

    struct FGunnerAnimationWorld
    {
        UWorld* World = nullptr;
        ATestCharacter* Player = nullptr;
        AGunWeaponActor* Gun = nullptr;
        USkeletalMeshComponent* Mesh = nullptr;
        USkeletalMeshComponent* GunMesh = nullptr;
        const uint64 InitialFrame = GFrameCounter;

        FGunnerAnimationWorld()
        {
            World = UWorld::CreateWorld(EWorldType::GamePreview, false);
            GEngine->CreateNewWorldContext(EWorldType::GamePreview).SetCurrentWorld(World);
            World->InitializeActorsForPlay(FURL());
        }

        ~FGunnerAnimationWorld()
        {
            World->DestroyWorld(false);
            GEngine->DestroyWorldContext(World);
            GFrameCounter = InitialFrame;
        }

        bool Spawn(const TCHAR* GunName)
        {
            UClass* PlayerClass = LoadClass<ATestCharacter>(nullptr,
                TEXT("/Game/BluePrints/Test/Player/BP_PlayerGunner.BP_PlayerGunner_C"));
            const FString GunPath = FString::Printf(TEXT("/Game/BluePrints/Weapon/Gun/%s.%s_C"), GunName, GunName);
            UClass* GunClass = LoadClass<AGunWeaponActor>(nullptr, *GunPath);
            if (!PlayerClass || !GunClass) return false;
            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
            Player = World->SpawnActor<ATestCharacter>(PlayerClass, FVector(0, 0, 100), FRotator::ZeroRotator, Params);
            Gun = World->SpawnActor<AGunWeaponActor>(GunClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
            if (!Player || !Gun) return false;
            Mesh = Player->GetMesh();
            GunMesh = Gun->FindComponentByClass<USkeletalMeshComponent>();
            if (!Mesh || !Mesh->GetSkeletalMeshAsset() || !Mesh->GetAnimInstance() || !GunMesh || !GunMesh->GetSkeletalMeshAsset()) return false;
            World->SpawnActor<APlayerController>()->Possess(Player);
            Player->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
            Mesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
            Mesh->bEnableUpdateRateOptimizations = false;
            Gun->SetOwner(Player);
            Gun->SetInstigator(Player);
            Gun->SetActorEnableCollision(false);
            Gun->SetOwnerComponent(Player->GetWeaponComponent());
            SetGunnerObject(Player->GetWeaponComponent(), TEXT("Owner"), Player);
            SetGunnerObject(Player->GetWeaponComponent(), TEXT("CurrentWeapon"), Gun);
            SetGunnerObject(Player->GetWeaponComponent(), TEXT("ActivatedWeapon"), Gun);
            Gun->AttachToComponent(Mesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Weapon_Gun"));
            // Avoid unrelated game-instance/UI BeginPlay, but provide the ABP's actual weapon-dependent IK input.
            SetGunnerObject(Mesh->GetAnimInstance(), TEXT("PlayerCharacter"), Player);
            Player->SetPlayerActivatedWeapon(EWeaponType::Gun);
            return true;
        }

        UAnimMontage* Montage() const
        {
            auto* Property = FindFProperty<FObjectPropertyBase>(Player->GetClass(), TEXT("AttackMontage_Gun"));
            return Property ? Cast<UAnimMontage>(Property->GetObjectPropertyValue_InContainer(Player)) : nullptr;
        }

        void Step()
        {
            ++GFrameCounter;
            Mesh->TickAnimation(GunnerPoseStep, false);
            Mesh->RefreshBoneTransforms();
            Mesh->ConditionallyDispatchQueuedAnimEvents();
            GunMesh->TickAnimation(GunnerPoseStep, false);
            GunMesh->RefreshBoneTransforms();
        }
    };

    struct FBoneLengthSamples
    {
        FName Name;
        int32 Index = INDEX_NONE;
        double ReferenceLength = 0.0;
        double MinimumRatio = DBL_MAX;
        double MaximumRatio = 0.0;

        void Record(const FTransform& Transform)
        {
            const double Ratio = Transform.GetTranslation().Length() / ReferenceLength;
            MinimumRatio = FMath::Min(MinimumRatio, Ratio);
            MaximumRatio = FMath::Max(MaximumRatio, Ratio);
        }
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPlayerGunnerPoseTest, "TeamPotato.Combat.PlayerGunner.ShootingPreservesBodyProportions",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FPlayerGunnerPoseTest::RunTest(const FString& Parameters)
{
    for (const TCHAR* GunName : {TEXT("BP_Gun_M16"), TEXT("BP_Gun_AK47")})
    {
        FGunnerAnimationWorld Fixture;
        if (!TestTrue(FString(GunName) + TEXT(" equips the real Gunner mesh, ABP and weapon"), Fixture.Spawn(GunName))) continue;
        TestTrue(TEXT("Player weapon attachment socket exists"), Fixture.Mesh->DoesSocketExist(TEXT("Weapon_Gun")));
        TestTrue(TEXT("Weapon supplies a real left-hand grip socket"), Fixture.GunMesh->DoesSocketExist(TEXT("GrapPoint")));
        TestEqual(TEXT("Gun is attached to the player weapon socket"), Fixture.Gun->GetAttachParentSocketName(), FName(TEXT("Weapon_Gun")));
        TestEqual(TEXT("The ABP resolves the equipped gun"), Fixture.Player->GetWeaponComponent()->GetActivateWeapon(), static_cast<AWeaponBase*>(Fixture.Gun));
        UAnimMontage* Montage = Fixture.Montage();
        if (!TestNotNull(TEXT("Gunner has a firing montage"), Montage)) continue;
        USkeleton* TargetSkeleton = Fixture.Mesh->GetSkeletalMeshAsset()->GetSkeleton();
        TestEqual(TEXT("Firing montage is authored for the Gunner skeleton"), Montage->GetSkeleton(), TargetSkeleton);
        int32 SequenceCount = 0;
        for (const FSlotAnimationTrack& Track : Montage->SlotAnimTracks)
        {
            for (const FAnimSegment& Segment : Track.AnimTrack.AnimSegments)
            {
                auto* Sequence = Cast<UAnimSequence>(Segment.GetAnimReference());
                if (!TestNotNull(TEXT("Firing montage contains a sequence"), Sequence)) continue;
                ++SequenceCount;
                TestEqual(TEXT("Firing sequence uses the Gunner skeleton"), Sequence->GetSkeleton(), TargetSkeleton);
            }
        }
        TestTrue(TEXT("Firing montage has animation content"), SequenceCount > 0);

        const FReferenceSkeleton& Reference = Fixture.Mesh->GetSkeletalMeshAsset()->GetRefSkeleton();
        TArray<FBoneLengthSamples> Bones;
        for (const TCHAR* BoneName : {TEXT("spine_01"), TEXT("spine_02"), TEXT("spine_03"),
            TEXT("upperarm_l"), TEXT("lowerarm_l"), TEXT("hand_l"), TEXT("upperarm_r"), TEXT("lowerarm_r"), TEXT("hand_r")})
        {
            const int32 Index = Reference.FindBoneIndex(BoneName);
            if (!TestTrue(FString(BoneName) + TEXT(" exists in the Gunner reference skeleton"), Index != INDEX_NONE)) continue;
            const double Length = Reference.GetRefBonePose()[Index].GetTranslation().Length();
            if (!TestTrue(FString(BoneName) + TEXT(" has a measurable reference segment"), Length > 0.1)) continue;
            FBoneLengthSamples& Bone = Bones.AddDefaulted_GetRef();
            Bone.Name = BoneName;
            Bone.Index = Index;
            Bone.ReferenceLength = Length;
        }
        auto RecordPose = [&]()
        {
            const TArrayView<const FTransform> Pose = Fixture.Mesh->GetBoneSpaceTransformsView();
            for (FBoneLengthSamples& Bone : Bones)
                if (Pose.IsValidIndex(Bone.Index)) Bone.Record(Pose[Bone.Index]);
        };

        // Settle the weapon-state transition, then include idle, the entire shot and the return blend.
        for (int32 Frame = 0; Frame < 60; ++Frame) Fixture.Step();
        auto* GunActive = FindFProperty<FBoolProperty>(Fixture.Mesh->GetAnimInstance()->GetClass(), TEXT("bIsGunActivated"));
        TestTrue(TEXT("Actual ABP evaluates its gun state and hand IK"), GunActive && GunActive->GetPropertyValue_InContainer(Fixture.Mesh->GetAnimInstance()));
        for (int32 Frame = 0; Frame < 30; ++Frame)
        {
            Fixture.Step();
            RecordPose();
        }
        const FTransform IdleHand = Fixture.Mesh->GetSocketTransform(TEXT("hand_r"), RTS_Component);
        Fixture.Player->PlayGunShootingMontage();
        TestTrue(TEXT("Native firing entry point starts its configured montage"), Fixture.Mesh->GetAnimInstance()->Montage_IsPlaying(Montage));
        const int32 Frames = FMath::CeilToInt((Montage->GetPlayLength() + 0.5f) / GunnerPoseStep);
        double MaximumHandTravel = 0.0, MaximumHandRotation = 0.0;
        int32 ActiveFrames = 0;
        for (int32 Frame = 0; Frame < Frames; ++Frame)
        {
            Fixture.Step();
            RecordPose();
            if (Fixture.Mesh->GetAnimInstance()->Montage_IsPlaying(Montage))
            {
                ++ActiveFrames;
                const FTransform Hand = Fixture.Mesh->GetSocketTransform(TEXT("hand_r"), RTS_Component);
                MaximumHandTravel = FMath::Max(MaximumHandTravel, FVector::Distance(Hand.GetTranslation(), IdleHand.GetTranslation()));
                MaximumHandRotation = FMath::Max(MaximumHandRotation, Hand.GetRotation().AngularDistance(IdleHand.GetRotation()));
            }
        }
        TestTrue(TEXT("Evaluates multiple frames during the actual firing montage"), ActiveFrames >= 10);
        TestTrue(TEXT("Firing changes the evaluated pose, rather than only advancing a playback timer"), MaximumHandTravel > 0.1 || MaximumHandRotation > 0.002);
        TestFalse(TEXT("Firing montage finishes naturally"), Fixture.Mesh->GetAnimInstance()->Montage_IsActive(Montage));
        for (const FBoneLengthSamples& Bone : Bones)
        {
            const FString Label = FString::Printf(TEXT("%s %s"), GunName, *Bone.Name.ToString());
            AddInfo(FString::Printf(TEXT("%s: reference %.3f cm, evaluated length ratio %.5f..%.5f"),
                *Label, Bone.ReferenceLength, Bone.MinimumRatio, Bone.MaximumRatio));
            TestTrue(Label + TEXT(" remains within 5 percent of its reference length through idle and firing"),
                Bone.MinimumRatio >= 0.95 && Bone.MaximumRatio <= 1.05);
        }
    }
    return true;
}

#endif
