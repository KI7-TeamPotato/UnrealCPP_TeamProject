// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Enemy/EnemyHealthBarWidget.h"
#include "Components/ProgressBar.h"

void UEnemyHealthBarWidget::SetHealthPercent(float Percent)
{
    if (EnemyHealthBar)
    {
        EnemyHealthBar->SetPercent(Percent);
    }
}
