// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Data/EnumBase.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPOTATO_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
    //시작시 호출되는 함수
	virtual void NativeInitializeAnimation() override;
    //애니메이션 업데이트 시마다 호출되는 함수
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

protected:
    //플레이어의 이동속도
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float Speed = 0.0f;

    //검 활성화 여부
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
    bool bIsSwordActivated = false;

    //총 활성화 여부
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
    bool bIsGunActivated = false;

    //위아래 시야각
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sight")
    float SightDegree = 0.0f;

    //앞으로 가는지 뒤로 가는지, true일때 앞, false일때 뒤
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    float playerFrontDirection = 0.0f;

    //좌우 방향(0~2, 0일때 왼쪽, 2일때 오른쪽)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    float PlayerSideDirection = 0.0f;

private:
    //플레이어 무빙 컴포넌트
	TWeakObjectPtr<const UPawnMovementComponent> OwnerMovementComponent = nullptr;

    //플레이어
	TObjectPtr<class ATestCharacter> MainPlayer = nullptr;

    //플레이어가 움직이는 방향
    FVector2D PlayerMovingDirection;

    //값을 반전하기 위한 -1상수
    const float Reverse = -1;
};
