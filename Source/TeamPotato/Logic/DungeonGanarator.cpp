// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonGanarator.h"
#include "TeamPotato/Room/RoomBase.h"
#include "TeamPotato/Room/DungeonRoom1.h"
#include "TeamPotato/Logic/ClosingWall.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
#include "Door.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy/EnemyCharacter.h"
#include "Item/Weapon/WeaponBoxActor.h"
#include "Subsystem/MVVMSubsystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/GameInstance.h"

// Sets default values
ADungeonGanarator::ADungeonGanarator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ADungeonGanarator::BeginPlay()
{
	Super::BeginPlay();

    // MVVM 서브시스템에 자신을 등록
    if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
    {
		if (UMVVMSubsystem* Subsystem = GameInstance->GetSubsystem<UMVVMSubsystem>())
		{
			Subsystem->RegisterDungeonGeneratorActor(this);
		}
    }

    //시드 정하기
    SetSeed();

    StageConfigSetting();

    if (!ValidateGenerationConfig() || !SpawnStarterRooms())
    {
        UE_LOG(LogTemp, Error, TEXT("Dungeon generation aborted because required configuration is invalid."));
        return;
    }

    bIsFinalizingDungeon = false;
    bIsDungeonGenerationCompleted = false;

    // 생성이 끝나지 못하는 경우에만 재시도하는 안전장치
    GetWorld()->GetTimerManager().SetTimer(GenerationTimeoutHandle, this, &ADungeonGanarator::OnGenerationTimeout, 4.0f, false);

    //시작 방 생성
	SpawnNextRoom();
}

void ADungeonGanarator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
    }

    if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
    {
		if (UMVVMSubsystem* Subsystem = GameInstance->GetSubsystem<UMVVMSubsystem>())
		{
			Subsystem->UnregisterDungeonGeneratorActor(this);
		}
    }

    Super::EndPlay(EndPlayReason);
}

bool ADungeonGanarator::SpawnStarterRooms()
{
    Exits.Empty();

    if (!GetWorld() || StartRoom.IsEmpty() || !StartRoom[0])
    {
        UE_LOG(LogTemp, Error, TEXT("StartRoom 배열이 비어있거나 유효한 클래스가 없습니다!"));
        return false;
    }

    //시작 방 생성
    ARoomBase* SpawnStartRoom = this->GetWorld()->SpawnActor<ARoomBase>(StartRoom[0]);
	if (IsValid(SpawnStartRoom) && IsValid(SpawnStartRoom->ExitPointsFolder))
	{
		SpawnStartRoom->SetActorLocation(this->GetActorLocation());

		SpawnStartRoom->ExitPointsFolder->GetChildrenComponents(false, Exits);
		Exits.RemoveAll([](const USceneComponent* Exit) { return !IsValid(Exit); });

		if (Exits.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("Spawned start room has no valid exit points."));
			SpawnStartRoom->Destroy();
			return false;
		}

        //리셋을 위해 따로 저장
        GeneratedActors.Add(SpawnStartRoom);
		LastestSpawnRoom = SpawnStartRoom;
		return true;
	}

	if (IsValid(SpawnStartRoom))
	{
		SpawnStartRoom->Destroy();
	}
	UE_LOG(LogTemp, Error, TEXT("Failed to spawn a valid start room."));
	return false;
}

void ADungeonGanarator::SpawnNextRoom()
{
    if (bIsFinalizingDungeon || bIsDungeonGenerationCompleted)
    {
        return;
    }

    if (RoomAmount <= 0)
    {
        AfterEndedSpawnNomalRooms();
        return;
    }

    if (!GetWorld() || CorridorRooms.IsEmpty() || RoomsToBeSpawned.IsEmpty() || Exits.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot continue dungeon generation: required rooms or exits are missing."));
        ResetDungeon();
        return;
    }

    bCanSpawn = true;

    //시작 방을 기준으로 시작 방의 출구(Exits)에 복도를 생성, 복도 생성에 성공하면 방을 생성
    //시작 방 생성 -> 복도 생성 시도-> 복도 생성 가능한지 검사 -> 생성 가능하면 방 생성 시도 -> 방 생성 가능한지 검사 -> 생성 가능하면 방 생성 -> RoomAmount != 0이면(아직 생성할 방이 남았으면) 0.01초 후(바로 호출하면 좀 불안정함) 다시 SpawnNextRoom 실행 
    //                                                     ㄴ> 생성 실패하면 Destroy                            ㄴ> 생성 실패하면 복도와 같이 Destroy
    //오버랩 판단과 제거는  RemoveOverlappingRooms()로 함

    //복도 생성
    int32 ExitIndex = RandomStream.RandRange(0, Exits.Num() - 1);
    USceneComponent* SelectedExitPoint = Exits[ExitIndex];
	if (!IsValid(SelectedExitPoint))
	{
		Exits.RemoveAt(ExitIndex);
		ScheduleNextRoomSpawn();
		return;
	}

    int32 RandomCorridorIndex = RandomStream.RandRange(0, CorridorRooms.Num() - 1);
    TSubclassOf<ARoomBase> SelectedCorridorClass = CorridorRooms[RandomCorridorIndex];
	if (!SelectedCorridorClass)
	{
		UE_LOG(LogTemp, Error, TEXT("Selected corridor class is invalid."));
		ScheduleNextRoomSpawn();
		return;
	}

    ARoomBase* SpawnedCorridor = this->GetWorld()->SpawnActor<ARoomBase>(SelectedCorridorClass);

    if (SpawnedCorridor)
    {
        //복도도 생성 목록에 등록
        GeneratedActors.Add(SpawnedCorridor);
    }
    else
    {
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn corridor. Retrying dungeon room generation."));
		ScheduleNextRoomSpawn();
        return;
    }

    // 위치/회전 설정
    SpawnedCorridor->SetActorLocation(SelectedExitPoint->GetComponentLocation());
    SpawnedCorridor->SetActorRotation(SelectedExitPoint->GetComponentRotation());

    // 복도 오버랩 검사
    LastestSpawnRoom = SpawnedCorridor;
    const bool bCorridorPlacementValid = RemoveOverlappingRooms();//오버랩 있으면 제거
	if (!bCorridorPlacementValid && IsValid(SpawnedCorridor))
	{
		SpawnedCorridor->Destroy();
	}

    // 복도가 겹쳐서 파괴되었다면 리턴
    if (!IsValid(SpawnedCorridor))
    {
		GeneratedActors.Remove(SpawnedCorridor);
		ScheduleNextRoomSpawn();
        return;//0.01초 사이에 아래 있는 방 생성 코드가 호출됨 그래서 retrun으로 빠르게 컷 해야함
    }

    TArray<USceneComponent*> CorridorExits;
	if (IsValid(SpawnedCorridor->ExitPointsFolder))
	{
		SpawnedCorridor->ExitPointsFolder->GetChildrenComponents(false, CorridorExits);
	}
	CorridorExits.RemoveAll([](const USceneComponent* Exit) { return !IsValid(Exit); });

    if (CorridorExits.IsEmpty())
    {
		GeneratedActors.Remove(SpawnedCorridor);
        SpawnedCorridor->Destroy();
		ScheduleNextRoomSpawn();
        return;
    }

    USceneComponent* CorridorExitPoint = CorridorExits[0];

    //방 생성
    TSubclassOf<ARoomBase> RoomClassToSpawn;
    IsSpawnSpecialRoom = false;

    // N번째 방마다 특수 방 생성 (배열이 비어있는지 체크 후 생성, N은 임의의 정수)
    if (RoomAmount % 3 == 0 && SpecialRoomsToBeSpawned.Num() > 0)
    {
        SpecialRoomIndex = CurrentSpecialRoomIndex % SpecialRoomsToBeSpawned.Num();

        RoomClassToSpawn = SpecialRoomsToBeSpawned[SpecialRoomIndex];
        UE_LOG(LogTemp, Warning, TEXT("%d"), CurrentSpecialRoomIndex);
        IsSpawnSpecialRoom = true;

        //버그 있음 가끔 같은 특수방이 나옴 Seed = 13190 -> 해결함
    }
    else
    {
        int32 RoomIndex = RandomStream.RandRange(0, RoomsToBeSpawned.Num() - 1);
        RoomClassToSpawn = RoomsToBeSpawned[RoomIndex];
    }

	if (!RoomClassToSpawn)
	{
		GeneratedActors.Remove(SpawnedCorridor);
		SpawnedCorridor->Destroy();
		UE_LOG(LogTemp, Error, TEXT("Selected room class is invalid."));
		ScheduleNextRoomSpawn();
		return;
	}

    ARoomBase* SpawnedRoom = this->GetWorld()->SpawnActor<ARoomBase>(RoomClassToSpawn);

    if (!IsValid(SpawnedRoom))
    {
		GeneratedActors.Remove(SpawnedCorridor);
		SpawnedCorridor->Destroy();
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn room. Retrying dungeon room generation."));
		ScheduleNextRoomSpawn();
		return;
    }

    SpawnedRoom->SetActorLocation(CorridorExitPoint->GetComponentLocation());
    SpawnedRoom->SetActorRotation(CorridorExitPoint->GetComponentRotation());

    // 방 오버랩 검사
    LastestSpawnRoom = SpawnedRoom;

    const bool bRoomPlacementValid = RemoveOverlappingRooms();
	if (!bRoomPlacementValid && IsValid(SpawnedRoom))
	{
		SpawnedRoom->Destroy();
	}

    if (bRoomPlacementValid && IsValid(SpawnedRoom))
    {
		GeneratedActors.Add(SpawnedRoom);
		DoorList.Add(SelectedExitPoint);
		DoorList.Add(CorridorExitPoint); // 2. 복도 -> 다음 방 사이의 문

        // 성공: 기존 출구 제거 및 새 방 출구 추가
        Exits.Remove(SelectedExitPoint);
        if (IsSpawnSpecialRoom)
        {
            CurrentSpecialRoomIndex++;
        }
        RoomAmount--;
        TArray<USceneComponent*> NewRoomExits;
		if (IsValid(SpawnedRoom->ExitPointsFolder))
		{
			SpawnedRoom->ExitPointsFolder->GetChildrenComponents(false, NewRoomExits);
		}
		NewRoomExits.RemoveAll([](const USceneComponent* Exit) { return !IsValid(Exit); });
        Exits.Append(NewRoomExits);
    }
    else
    {
        // 실패: 방이 겹치면 연결된 복도도 같이 파괴
        if (IsValid(SpawnedCorridor))
        {
		   GeneratedActors.Remove(SpawnedCorridor);
           SpawnedCorridor->Destroy();
        }
    }

    if (RoomAmount > 0)
    {
		ScheduleNextRoomSpawn();
    }
	else
	{
		AfterEndedSpawnNomalRooms();
	}
}

bool ADungeonGanarator::RemoveOverlappingRooms()
{
    if (!IsValid(LastestSpawnRoom) || !IsValid(LastestSpawnRoom->OverlapFolder))
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot test room overlap because the room or overlap folder is invalid."));
		return false;
	}

    //오버랩 된 방 제거
	TArray<USceneComponent*> OverlappedRooms;
	LastestSpawnRoom->OverlapFolder->GetChildrenComponents(false, OverlappedRooms);

	TArray<UPrimitiveComponent*> OverlapingCompoenets;

	for (USceneComponent* Element : OverlappedRooms)
	{
		if (UBoxComponent* OverlapBox = Cast<UBoxComponent>(Element))
		{
			OverlapBox->GetOverlappingComponents(OverlapingCompoenets);
		}
	}

	for (USceneComponent* Element : OverlapingCompoenets)
	{
		bCanSpawn = false;
		//RoomAmount++;
		LastestSpawnRoom->Destroy();
		return false;
	}

	return true;
}

void ADungeonGanarator::ScheduleNextRoomSpawn()
{
	if (!GetWorld() || bIsFinalizingDungeon || bIsDungeonGenerationCompleted)
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(RoomSpawnTimerHandle, this, &ADungeonGanarator::SpawnNextRoom, 0.01f, false);
}

bool ADungeonGanarator::ValidateGenerationConfig() const
{
	const TArray<TSubclassOf<ARoomBase>>& LastRoomClasses = chapter == MaxAmout ? BossRoomClass : PotalRoomClass;
	return GetWorld()
		&& !StartRoom.IsEmpty() && StartRoom[0]
		&& !RoomsToBeSpawned.IsEmpty()
		&& !CorridorRooms.IsEmpty()
		&& !ClosingWall.IsEmpty() && ClosingWall[0]
		&& !LastRoomClasses.IsEmpty() && LastRoomClasses[0]
		&& RoomAmount > 0;
}

//모든 방 생성이 끝나고 실행되는 함수, 보스방 스폰과 닫힌 벽을 막고 방을 들어갔는지 확인하는 콜리전 활성화함
void ADungeonGanarator::AfterEndedSpawnNomalRooms()
{
    if (!GetWorld() || bIsFinalizingDungeon || bIsDungeonGenerationCompleted || RoomAmount > 0)
    {
        return;
    }

    bIsFinalizingDungeon = true;
    GetWorld()->GetTimerManager().ClearTimer(RoomSpawnTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(GenerationTimeoutHandle);

    //보물방 같은 특수방 지정하는 함수, 단순히 현재 생성된 방들 중 고를거라 보스방 생성 전 돌려야함 아마 사용 안할듯
    //SelectedSpecialRoom();
    //보스방 생성
    if (!SpawnLastRoom())
    {
        bIsFinalizingDungeon = false;
        UE_LOG(LogTemp, Warning, TEXT("Failed to spawn the final room. Regenerating dungeon."));
        ResetDungeon();
        return;
        //통로 닫기 전 보스방 생성 성공여부 검사, 생성 실패하면 스테이지 다시 만드는 구조라 ClosingUnuusedWall 호출되지 않게 리턴
    }
    //안쓰는 통로 닫는 함수
    SpawnDoors();

    ClosingUnuusedWall();

    for (AActor* Actor : GeneratedActors)
    {
        ARoomBase* Room = Cast<ARoomBase>(Actor);
        if (Room)
        {
            Room->ActivateBattleTrigger();
        }
    }


    /// 보스방 생성 성공시에 처리(Minimap)
    CalculateDungeonMinMaxPoint();

    bIsDungeonGenerationCompleted = true;
    bIsFinalizingDungeon = false;

    if (EndedCreate.IsBound())
    {
        EndedCreate.Broadcast();
    }

    // 스테이지 및 챕터 변경 델리게이트 호출
    OnStageAndChapterChanged.Broadcast(Stage, chapter);

}

//닫힌 벽 막는 함수
void ADungeonGanarator::ClosingUnuusedWall()
{
    if (!GetWorld() || ClosingWall.IsEmpty() || !ClosingWall[0])
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot close unused exits: closing wall class is invalid."));
        return;
    }

    //모든 출구에 대해 검사
    for (USceneComponent* Element : Exits)
    {
		if (!IsValid(Element))
		{
			continue;
		}

        //막을 벽 설정
        AClosingWall* LastestClosingWallSpawned = GetWorld()->SpawnActor<AClosingWall>(ClosingWall[0]);
		if (!IsValid(LastestClosingWallSpawned))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn a closing wall."));
			continue;
		}

        FVector RelativeOffset(LastestClosingWallSpawned->GetLoc());//여기 나중에 수정해야 함(하드코딩)
        FVector WorldOffset = Element->GetComponentRotation().RotateVector(RelativeOffset);

        LastestClosingWallSpawned->SetActorLocation(Element->GetComponentLocation() + WorldOffset);
        LastestClosingWallSpawned->SetActorRotation(Element->GetComponentRotation() + LastestClosingWallSpawned->GetRot());

        GeneratedActors.Add(LastestClosingWallSpawned);
    }
    UE_LOG(LogTemp, Warning, TEXT("ClosingUnuusedWall"));
}

bool ADungeonGanarator::SpawnLastRoom()
{
    //보스방 생성
    const TArray<TSubclassOf<ARoomBase>>* TargetRoomArray = nullptr;

    if (chapter == MaxAmout)
    {
        TargetRoomArray = &BossRoomClass;
        UE_LOG(LogTemp, Warning, TEXT("createboss"));
    }
    else
    {
        TargetRoomArray = &PotalRoomClass;
        UE_LOG(LogTemp, Warning, TEXT("createportal"));
    }

    if (!GetWorld() || !TargetRoomArray || TargetRoomArray->IsEmpty() || !(*TargetRoomArray)[0] || !IsValid(LastestSpawnRoom))
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot spawn final room: class or latest room is invalid."));
        return false;
    }

    USceneComponent* LastExit = nullptr;
    for (USceneComponent* Exit : Exits)
    {
        if (IsValid(Exit) && Exit->GetOwner() == LastestSpawnRoom)
        {
			LastExit = Exit;
			break;
        }
    }

    if (!IsValid(LastExit))
    {
		UE_LOG(LogTemp, Error, TEXT("Cannot spawn final room: latest room has no available exit."));
		return false;
	}

    ARoomBase* LastRoom = GetWorld()->SpawnActor<ARoomBase>((*TargetRoomArray)[0]);
    if (!IsValid(LastRoom))
    {
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn final room actor."));
		return false;
	}

    LastRoom->SetActorLocation(LastExit->GetComponentLocation());
    LastRoom->SetActorRotation(LastExit->GetComponentRotation());

    ARoomBase* PreviousLastRoom = LastestSpawnRoom;
    LastestSpawnRoom = LastRoom;
    if (!RemoveOverlappingRooms() || !IsValid(LastRoom))
    {
		LastestSpawnRoom = PreviousLastRoom;
		if (IsValid(LastRoom))
		{
			LastRoom->Destroy();
		}
		return false;
	}

    DoorList.Add(LastExit);
    GeneratedActors.Add(LastRoom);
    Exits.Remove(LastExit);
    UE_LOG(LogTemp, Warning, TEXT("Final room spawned successfully."));
    CurrentResetCount = 0;
    return true;
}

void ADungeonGanarator::SelectedSpecialRoom()
{
    //UE_LOG(LogTemp, Log, TEXT("SelectedSpecialRoom"));

}

void ADungeonGanarator::ResetDungeon()
{
    if (!GetWorld())
    {
        return;
    }

    if (CurrentResetCount >= MaxResetLimit)
    {
        GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
        UE_LOG(LogTemp, Error, TEXT("Dungeon generation failed after %d reset attempts."), MaxResetLimit);
        return;
    }

    //현재 진행 중인 타이머 모두 중지
    GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

    TArray<AActor*> AllEnemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), AllEnemies);
    TArray<AActor*> WeaponBoxes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWeaponBoxActor::StaticClass(), WeaponBoxes);

    StageConfigSetting();
    for (AActor* EnemyActor : AllEnemies)
    {
        if (IsValid(EnemyActor))
        {
            EnemyActor->Destroy();
        }
    }

    for (AActor* BoxActor : WeaponBoxes)
    {
        if (IsValid(BoxActor))
        {
            BoxActor->Destroy();
        }
    }

    //지금까지 생성된 모든 방/복도 파괴

    for (AActor* Actor : GeneratedActors)
    {
        if (IsValid(Actor))
        {
            Actor->Destroy();
        }
    }
    GeneratedActors.Empty(); // 목록 비우기
    Exits.Empty();           // 출구 목록 비우기
    DoorList.Empty();
	LastestSpawnRoom = nullptr;
    //변수 초기화
    CurrentSpecialRoomIndex = 0;
    SpecialRoomIndex = 0;
    RoomAmount = InitialRoomAmount; // 방 개수 복구
    bCanSpawn = false;
	bIsFinalizingDungeon = false;
	bIsDungeonGenerationCompleted = false;
    CurrentResetCount++;

	if (!ValidateGenerationConfig() || !SpawnStarterRooms())
	{
		UE_LOG(LogTemp, Error, TEXT("Dungeon regeneration aborted because required configuration is invalid."));
		return;
	}

    GetWorld()->GetTimerManager().SetTimer(GenerationTimeoutHandle, this, &ADungeonGanarator::OnGenerationTimeout, 4.0f, false);
    SpawnNextRoom();
}

void ADungeonGanarator::OnGenerationTimeout()
{
    if (!bIsDungeonGenerationCompleted && !bIsFinalizingDungeon)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dungeon generation timed out. Regenerating dungeon."));
		ResetDungeon();
	}
}

void ADungeonGanarator::SetSeed()//게임에 사용할 시드 정하는 함수
{
    int32 Results;
    if (Seed == -1)//-1이면 랜덤 시드
    {
        Results = FMath::Rand();
    }
    else //아니면 미리 입력한 시드
    {
        Results = Seed;
    }
    RandomStream.Initialize(Results);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%d"), Results));//디버그용 시드 출력
	}
}

void ADungeonGanarator::SpawnDoors()
{
    if (!GetWorld() || !Doors)
    {
        UE_LOG(LogTemp, Error, TEXT("Door is Null"));
        return;
    }

    // 모든 출구에 대해 검사
    for (USceneComponent* Element : DoorList)
    {
        if (!IsValid(Element)) continue;

        ADoor* LastestDoorSpawn = GetWorld()->SpawnActor<ADoor>(Doors);

        if (LastestDoorSpawn)
        {
            FVector RelativeOffset(LastestDoorSpawn->GetLoc());
            FVector WorldOffset = Element->GetComponentRotation().RotateVector(RelativeOffset);

            LastestDoorSpawn->SetActorLocation(Element->GetComponentLocation() + WorldOffset);
            LastestDoorSpawn->SetActorRotation(Element->GetComponentRotation() + LastestDoorSpawn->GetRot());

            GeneratedActors.Add(LastestDoorSpawn);

            AActor* OwnerActor = Element->GetOwner();
            ARoomBase* OwnerRoom = Cast<ARoomBase>(OwnerActor);

            if (OwnerRoom)
            {
                // 방에 문을 등록 (이 함수 안에서 Door->SetOwningRoom도 호출됨)
                OwnerRoom->RegisterDoor(LastestDoorSpawn);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("SpawnDoors"));
        }
    }

}

void ADungeonGanarator::StageConfigSetting()
{
    if (StageConfigMap.Contains(Stage))
    {
        FStageRoomConfig& SelectedConfig = StageConfigMap[Stage];
        RoomAmount = SelectedConfig.StageRoomAmount;

        // 1. 일반 방 목록 교체
        if (SelectedConfig.NormalRooms.Num() > 0
            && SelectedConfig.SpecialRooms.Num() > 0
            && SelectedConfig.Corridors.Num() > 0
            && SelectedConfig.StartRooms.Num() > 0
            && SelectedConfig.BossRooms.Num() > 0
            && SelectedConfig.ClosingWalls.Num() > 0
            && SelectedConfig.PotalRooms.Num() > 0
            && RoomAmount > 0
            )
        {
            RoomsToBeSpawned = SelectedConfig.NormalRooms;
            SpecialRoomsToBeSpawned = SelectedConfig.SpecialRooms;
            CorridorRooms = SelectedConfig.Corridors;
            StartRoom = SelectedConfig.StartRooms;
            BossRoomClass = SelectedConfig.BossRooms;
            PotalRoomClass = SelectedConfig.PotalRooms;
            ClosingWall = SelectedConfig.ClosingWalls;
			if (!SelectedConfig.Doors.IsEmpty())
			{
				Doors = SelectedConfig.Doors[0];
			}
            InitialRoomAmount = RoomAmount;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Stage %d : NormalRooms array is empty! Using default."), Stage);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Stage %d config not found in StageConfigMap. Using default arrays."), Stage);
    }
}

void ADungeonGanarator::GoToNextStage(int32 NewChapter)
{
    SetSeed();
    int32 NextStage = NewChapter+1;
    UE_LOG(LogTemp, Error, TEXT("%d스테이지-%d챕터"), Stage, NewChapter);
    if (NextStage > MaxAmout)
    {
        chapter = 1;
        Stage++;
    }
    else
    {
        chapter++;
    }
    ResetDungeon();
}

void ADungeonGanarator::CalculateDungeonMinMaxPoint()
{
    FVector2D MinPoint(FLT_MAX, FLT_MAX);
    FVector2D MaxPoint(-FLT_MAX, -FLT_MAX);
	bool bFoundValidBounds = false;

    for (AActor* actor : GeneratedActors)
    {
        if (!IsValid(actor))
        {
            continue;
        }

        FVector2D actorMin, actorMax;

        FVector Origin, Extend;
        actor->GetActorBounds(true, Origin, Extend);
		bFoundValidBounds = true;

        actorMin.X = Origin.X - Extend.X;
        actorMin.Y = Origin.Y - Extend.Y;
        actorMax.X = Origin.X + Extend.X;
        actorMax.Y = Origin.Y + Extend.Y;

        MinPoint.X = FMath::Min(MinPoint.X, actorMin.X);
        MinPoint.Y = FMath::Min(MinPoint.Y, actorMin.Y);

        MaxPoint.X = FMath::Max(MaxPoint.X, actorMax.X);
        MaxPoint.Y = FMath::Max(MaxPoint.Y, actorMax.Y);
    }

    if (!bFoundValidBounds)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot calculate dungeon bounds because no generated actors are valid."));
		return;
	}

    // 로딩창 종료
    if (OnDungeonGenerationCompleted.IsBound())
    {
        OnDungeonGenerationCompleted.Broadcast(MinPoint, MaxPoint);
    }
}
