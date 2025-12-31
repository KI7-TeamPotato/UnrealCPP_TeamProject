// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Component/WaveComponent.h"
#include "TeamPotato/Logic/Door.h"
#include "Kismet/GameplayStatics.h"
#include "RoomBase.generated.h"

UCLASS()
class TEAMPOTATO_API ARoomBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoomBase();

    // 이 방에 소속된 문들을 등록하는 배열
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Room Info")
    TArray<ADoor*> RoomDoors;

    // 문을 등록하는 함수
    void RegisterDoor(ADoor* NewDoor);
    void SetGlobalDoorState(bool bShouldClose);
    // 모든 문을 동시에 여는 함수
    UFUNCTION(BlueprintCallable)
    void OpenAllRoomDoors();

    //모든 문을 동시에 닫는 함수
    UFUNCTION(BlueprintCallable)
    void CloseAllRoomDoors();

    //플레이어가 방에 들어왔는지 검사하는 콜리전(OnEnterRoomCollision)을 관리하는 함수
    void ActivateBattleTrigger();
    
    //클리어 한 방인지 넘겨주는 getter
    bool GetRoomClear() { return bIsRoomClear; }
    
    //전투 시작 여부를 넘겨주는 getter
    bool GetStartBattle() { return bIsStartBattle; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    //방 클리어시 호출되는 함수
    UFUNCTION()
    void HandleBattleEnd();

    //전투 시작 여부
    bool bIsStartBattle = false;

    //방 클리어 여부
    bool bIsRoomClear = false;

    // [추가] 플레이어가 방에 진입했는지 감지하는 오버랩 함수
    UFUNCTION()
    void OnRoomTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, 
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex, 
        bool bFromSweep, 
        const FHitResult& SweepResult);
public:	
    //방에 있는 웨이브 시스템 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UWaveComponent> WaveSystemComp;

    //방 구성 요소들(콜리전, 메시, 폴더 등)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> DefaultScenRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> GeometryFolder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> WallFolder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> CeilingFolder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> FloorFolder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> OtherArchitecture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> OverlapFolder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> ExitPointsFolder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SpawnPointsFolder;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<USceneComponent> DecorationFolder;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UActorComponent> Torch;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UArrowComponent> SpawnPoint1;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UArrowComponent> SpawnPoint2;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UArrowComponent> SpawnPoint3;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UArrowComponent> SpawnPoint4;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UArrowComponent> SpawnPoint5;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UArrowComponent> SpawnPoint6;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UArrowComponent> SpawnPoint7;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Wall1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Wall2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Wall3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Wall4;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Wall5;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Wall6;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Wall7;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Wall8;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Floor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Floor2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Floor3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Floor4;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Ceiling1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Ceiling2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Ceiling3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Ceiling4;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Piller1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Piller2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Piller3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Piller4;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DoorWay1;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DoorWay2;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DoorWay3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BoxCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UArrowComponent> Arrow;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBoxComponent> OnEnterRoomCollision;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Deco1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Deco2;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Deco3;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Deco4;
};
