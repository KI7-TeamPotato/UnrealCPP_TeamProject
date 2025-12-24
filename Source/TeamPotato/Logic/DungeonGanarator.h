// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonGanarator.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCreateDungeonEnded);

// 던전 생성 완료시에 최대, 최소 지점 알려주는 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDungeonGenerationCompleted, FVector2D, InMinPoint, FVector2D, InMaxPoint);

USTRUCT(BlueprintType)
struct FStageRoomConfig
{
	GENERATED_BODY()

	// 해당 스테이지에 나올 일반 방들
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<class ARoomBase>> NormalRooms;

	// 해당 스테이지에 나올 특수 방들
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<class ARoomBase>> SpecialRooms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<class ARoomBase>> Corridors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<class ARoomBase>> BossRooms;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<class ARoomBase>> PotalRooms;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<class AClosingWall>> ClosingWalls;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<class ARoomBase>> StartRooms;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSubclassOf<class ADoor>> Doors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StageRoomAmount = 10;

};


class ADungeonRoom1;
class ARoomBase;
class AClosingWall;
class ADoor;

UCLASS()
class TEAMPOTATO_API ADungeonGanarator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADungeonGanarator();

	//던전 크기
	UPROPERTY(EditAnywhere, Category = "DungeonInfo")
	int32 RoomAmount = 0;

	UPROPERTY(EditAnywhere, Category = "DungeonInfo")
	int32 Stage = 1;

    UPROPERTY(EditAnywhere, Category = "DungeonInfo")
    int32 chapter = 1;

	UPROPERTY(EditAnywhere, Category = "DungeonInfo|Config")
	TMap<int32, FStageRoomConfig> StageConfigMap;

	//시작방
	UPROPERTY(EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<ARoomBase>> StartRoom;

	//생성할 일반 방 목록
	UPROPERTY(EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<ARoomBase>> RoomsToBeSpawned;

	//보스방
	UPROPERTY(EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<ARoomBase>> BossRoomClass; // 연결할 보스 방 클래스

    UPROPERTY(EditAnywhere, Category = "Rooms")
    TArray<TSubclassOf<ARoomBase>> PotalRoomClass; // 연결할 포탈 방 클래스 

	//생성할 특수방 목록
	UPROPERTY(EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<ARoomBase>> SpecialRoomsToBeSpawned;

	//방 사이 복도
	UPROPERTY(EditAnywhere, Category = "Rooms")
	TArray<TSubclassOf<ARoomBase>> CorridorRooms;

	//던전의 모든 방이 생성 끝나면 뚫린 벽 막아주는 클래스
	UPROPERTY(EditAnywhere, Category = "Rooms|UnusedExits")
	TArray<TSubclassOf<AClosingWall>> ClosingWall;

    UPROPERTY(EditAnywhere, Category = "Rooms|Door")
    //TArray<TSubclassOf<ADoor>> Doors;
    TSubclassOf<ADoor> Doors;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnCreateDungeonEnded EndedCreate;

    UFUNCTION(BlueprintCallable)
    int32 GetChapter() const { return chapter; }

    //=================================
    //=================================
    //=================================
    // 던전 생성 완료 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "DungeonGanarator|Delegate")
    FOnDungeonGenerationCompleted OnDungeonGenerationCompleted;

protected:
	//마지막으로 생성된 방
	ARoomBase* LastestSpawnRoom = nullptr;

	//방이 생성이 가능한지 체크하는 변수
	bool bCanSpawn = false;

	//출구(다음방 통로)
	TArray<USceneComponent*> Exits;

	TArray<USceneComponent*> DoorList;

	//Seed 시스템용 랜덤스트림
	FRandomStream RandomStream;

	//게임에 사용할 시드
	UPROPERTY(EditAnywhere, Category = "DungeonInfo")
	int32 Seed = -1;

	//현재 특수방 인덱스
	int32 CurrentSpecialRoomIndex = 0;

	//총 특수방 인덱스
	int32 SpecialRoomIndex = 0;

	//지금이 특수방 생성 타이밍인지
	bool IsSpawnSpecialRoom = false;
protected:
	//시작방 생성하는 함수
	void SpawnStarterRooms();

	//다음 방 생성하는 함수 RoomAmount남아있으면 재귀호출로 계속 생성함
	void SpawnNextRoom();

	//방이나 복도 겹치면 지워버리고 다시 SpawnNextRoom호출해서 새로운 방 만드는 함수
	void RemoveOverlappingRooms();

	//모든 일반 방이 끝나고 나머지 작업 시작할댸 호출하는 함수
	void AfterEndedSpawnNomalRooms();

	//1. 보물방으로 사용할 방 선택
	void SelectedSpecialRoom();

	//보스방 생성
	bool SpawnLastRoom();

	//모든 작업 끝나고 빈 통로 닫기
	void ClosingUnuusedWall();

	//보스방 생성에 문제가 있을때 문제 없을때까지 리셋
    UFUNCTION(BlueprintCallable)
	void ResetDungeon();

	//무한루프 방지용 강제 리셋 함수
	void OnGenerationTimeout();

	//시드 설정하는 함수
	void SetSeed();

    void SpawnDoors();

    void StageConfigSetting();

    UFUNCTION(BlueprintCallable)
    void GoToNextStage(int32 NewChapter);

    //=================================
    //=================================
    //=================================
    //던전 크기 계산 함수
    void CalculateDungeonMinMaxPoint();

private:
	//벽이랑 복도등 모든 요소 저장하는 배열(ResetDungeon에서 맵 리셋할떄 씀)
    UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> GeneratedActors;

	//RoomAmount를 실시간으로 감소시키면서 방을 생성하기 때문에 RoomAmount의 초기값을 저장해야 함, 그ㅡ때쓰는 변수
	int32 InitialRoomAmount = 0;

	//현재 리셋 카운트
	int32 CurrentResetCount = 0;

	//최대 리셋 카운트(필요 없긴 함)
	const int32 MaxResetLimit = 40;

	//방지 타이머;
	FTimerHandle GenerationTimeoutHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
