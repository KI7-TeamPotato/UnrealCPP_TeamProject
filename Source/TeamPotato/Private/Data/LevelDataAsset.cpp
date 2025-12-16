// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/LevelDataAsset.h"

bool ULevelDataAsset::GetLevelDataByState(EGameState State, FLevelData& OutLevelData) const
{
	for (const FLevelData& Level : Levels)
	{
		if(Level.GameState == State)
		{
			OutLevelData = Level;
			return true;
		}
	}
	return false;
}

bool ULevelDataAsset::GetLevelDataByName(FName LevelName, FLevelData& OutLevelData) const
{
	for (const FLevelData& Level : Levels)
	{
		if (Level.LevelName == LevelName)
		{
			OutLevelData = Level;
			return true;
		}
	}
	return false;
}
