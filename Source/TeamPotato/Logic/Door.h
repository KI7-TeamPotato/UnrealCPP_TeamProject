// Door.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Door.generated.h"

class UBoxComponent;
class ARoomBase;

UCLASS()
class TEAMPOTATO_API ADoor : public AActor
{
    GENERATED_BODY()

public:
    ADoor();

    FVector GetLoc() { return DoorLocationOffset; }
    FRotator GetRot() { return DoorRotationOffset; }

    void OpenDoor();

    void CloseDoor();

    // 방 설정
    void SetOwningRoom(ARoomBase* NewRoom);

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

private:
    // 내부 이동 로직
    void MoveDoor(bool bOpen);

protected:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr<UStaticMeshComponent> Door;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    TObjectPtr <USceneComponent> DefaultSceneRoot;

    // 설정 변수들
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Door")
    FVector DoorLocationOffset = FVector(0.0f, 0.0f, 0.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings|Door")
    FRotator DoorRotationOffset = FRotator(0.0f, 0.0f, 0.0f);

    UPROPERTY(VisibleAnywhere, Category = "Config")
    ARoomBase* OwningRoom;

    // [로직 변수]
    FVector StartLocation; // 닫힌 위치 (배치된 위치)
    FVector EndLocation;   // 현재 이동 목표 위치

    bool bCanMove = false;
    float OpenSpeed = 5.0f;
};