// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/BossBase.h"


ABossBase::ABossBase()
{
    BulletPatternComp = CreateDefaultSubobject<UBulletHellComponent>(TEXT("BulletPatternComp"));
}

void ABossBase::BeginPlay()
{
}

float ABossBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    if (ActualDamage <= 0.0f || CurrentHealth <= 0.0f)
    {
        return 0.0f;
    }

    CurrentHealth -= ActualDamage;

    UE_LOG(LogTemp, Warning, TEXT("[%s] Took Damage: %f, HP: %f"), *GetName(), ActualDamage, CurrentHealth);

    if (CurrentPhase == 1 && CurrentHealth <= MaxHealth * 0.5f)
    {
        CurrentPhase = 2;
    }

    if (CurrentHealth <= 0.0f)
    {
        OnDie();
    }

    return ActualDamage;

}

void ABossBase::ExecutePattern(int32 PatternIndex)
{
    UE_LOG(LogTemp, Log, TEXT("BossPattern: %d"), PatternIndex);

    switch (PatternIndex)
    {
    case 1: PatternOne();
        break;
    case 2: PatternTwo();
        break;
    case 3: PatternThree(); 
        break;
    case 4: PatternFour(); 
        break;
    case 5: PatternFive();
        break;
    default: UE_LOG(LogTemp, Warning, TEXT("PatternInvalid")); break;
    }
}

void ABossBase::PatternOne_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("Pattern1"));
}

void ABossBase::PatternTwo_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("Pattern2"));
}

void ABossBase::PatternThree_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("Pattern3"));
}

void ABossBase::PatternFour_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("Pattern4"));
}

void ABossBase::PatternFive_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("Pattern5"));
}