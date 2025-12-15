// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
enum class EEnemyState : uint8
{
    //�� ���� ������
    Idle        UMETA(DisplayName = "Idle"),
    Patrol      UMETA(DisplayName = "Patrol"),
    Attack      UMETA(DisplayName = "Attack"),
    Hit         UMETA(DisplayName = "Hit"),
    Die         UMETA(DisplayName = "Die")
};

UENUM(BlueprintType)
enum class EEnemySence : uint8
{
    //�� ���� ������
    None        UMETA(DisplayName = "None"),
    See         UMETA(DisplayName = "Sight"),
    Hit         UMETA(DisplayName = "Hit"),
};

UENUM(BlueprintType)
enum class EEnemySpeed : uint8
{
    //�� �̵� ��� ������ �ֵ� ������
    Idle        UMETA(DisplayName = "IdleSpeed"),
    Walking     UMETA(DisplayName = "WalkingSpeed"),
    Jumping     UMETA(DisplayName = "JumpingSpeed"),
    Sprint      UMETA(DisplayName = "SprintSpeed"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    None	UMETA(DisplayName = "None"),
    Sword	UMETA(DisplayName = "Sword"),
    Gun     UMETA(DisplayName = "Gun"),
};

/**
 * 
 */
class TEAMPOTATO_API EnumBase
{

};
