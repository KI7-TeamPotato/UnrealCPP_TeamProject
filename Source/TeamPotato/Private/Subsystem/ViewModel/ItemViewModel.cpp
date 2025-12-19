// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ViewModel/ItemViewModel.h"

void  UItemViewModel::SetCurrentGold(int32 CurrentGold)
{
    if (OnPlayerGoldChanged.IsBound())
    {
        OnPlayerGoldChanged.Broadcast(CurrentGold);
    }
}
