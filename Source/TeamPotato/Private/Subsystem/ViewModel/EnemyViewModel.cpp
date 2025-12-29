// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/ViewModel/EnemyViewModel.h"

void UEnemyViewModel::SetBossHealth(float CurrentHealth, float MaxHealth)
{
    if (UpdateBossHealth.IsBound())
    {
        UpdateBossHealth.Broadcast(CurrentHealth, MaxHealth);
    }
}
