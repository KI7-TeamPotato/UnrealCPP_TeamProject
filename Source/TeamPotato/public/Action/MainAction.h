// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class TEAMPOTATO_API MainAction
{
public:
	MainAction();
	~MainAction();

	void Attack(float Damage, ACharacter* Target);
};
