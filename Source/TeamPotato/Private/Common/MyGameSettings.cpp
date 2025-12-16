// Fill out your copyright notice in the Description page of Project Settings.


#include "Common/MyGameSettings.h"

UMyGameSettings::UMyGameSettings()
{
	CategoryName = TEXT("Game");
	SectionName = TEXT("MyGameSettings");
}

UMyGameSettings* UMyGameSettings::Get()
{
	// 언리얼 CDO 방식 => 언리얼이 알아서 싱글톤처럼 관리해줌
	return GetMutableDefault< UMyGameSettings>();
}