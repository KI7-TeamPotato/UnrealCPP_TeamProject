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
    //플레이어의 이동속도
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float Speed = 0.0f;

    //플레이어가 움직이는 방향
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
    EMovingDirection PlayerMovingDirection = EMovingDirection::None;

    //검 활성화 여부
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
    bool bIsSwordActivated = false;

    //총 활성화 여부
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
    bool bIsGunActivated = false;

    //위아래 시야각
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Sight")
    float SightDegree = 0.0f;

private:
    //플레이어 무빙 컴포넌트
	TWeakObjectPtr<const UPawnMovementComponent> OwnerMovementComponent = nullptr;

    //플레이어
	TObjectPtr<class ATestCharacter> MainPlayer = nullptr;
};
