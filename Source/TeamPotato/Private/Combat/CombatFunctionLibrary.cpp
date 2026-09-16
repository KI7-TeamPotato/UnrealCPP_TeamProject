#include "Combat/CombatFunctionLibrary.h"
#include "Combat/CombatAbilitySystemComponent.h"
#include "Combat/CombatTags.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

float UCombatFunctionLibrary::ApplyCombatDamage(AActor* Target, float Damage, AActor* DamageCauser, AController* InstigatorController,
    TSubclassOf<UDamageType> DamageType)
{
    return ApplyCombatDamageWithHit(Target, Damage, DamageCauser, InstigatorController, nullptr, DamageType);
}

float UCombatFunctionLibrary::ApplyCombatDamageWithHit(AActor* Target, float Damage, AActor* DamageCauser,
    AController* InstigatorController, const FHitResult* Hit, TSubclassOf<UDamageType> DamageType)
{
    if (!IsValid(Target) || !Target->HasAuthority() || !Target->CanBeDamaged() || !FMath::IsFinite(Damage) || Damage <= 0.0f) return 0.0f;
    UAbilitySystemComponent* ExistingASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target, true);
    if (!ExistingASC)
    {
        // Legacy test actors/destructibles still use Unreal's normal damage events.
        return UGameplayStatics::ApplyDamage(Target, Damage, InstigatorController, DamageCauser, DamageType);
    }
    UCombatAbilitySystemComponent* TargetASC = Cast<UCombatAbilitySystemComponent>(ExistingASC);
    if (!TargetASC || !TargetASC->IsCombatInitialized() || TargetASC->IsDead() || TargetASC->GetHealth() <= 0.0f) return 0.0f;

    // Resolve the current source on every hit (including reused/pool projectiles).
    AActor* SourceActor = IsValid(InstigatorController) ? InstigatorController->GetPawn() : nullptr;
    if (!SourceActor && IsValid(DamageCauser)) SourceActor = DamageCauser->GetInstigator();
    if (!SourceActor) SourceActor = DamageCauser;
    UAbilitySystemComponent* SourceASC = nullptr;
    TSet<AActor*> Visited;
    while (IsValid(SourceActor) && !Visited.Contains(SourceActor))
    {
        Visited.Add(SourceActor);
        SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(SourceActor, true);
        if (SourceASC) break;
        SourceActor = SourceActor->GetOwner();
    }

    // World hazards may have no ASC. Retain their identity in the context.
    UAbilitySystemComponent* SpecASC = SourceASC ? SourceASC : TargetASC;
    FGameplayEffectContextHandle Context = SpecASC->MakeEffectContext();
    Context.AddInstigator(SourceASC ? SourceASC->GetAvatarActor() : DamageCauser, DamageCauser);
    if (IsValid(DamageCauser)) Context.AddSourceObject(DamageCauser);
    if (Hit) Context.AddHitResult(*Hit);
    const UCombatAbilitySystemComponent* CombatSource = Cast<UCombatAbilitySystemComponent>(SourceASC);
    const TSubclassOf<UGameplayEffect> DamageEffect = CombatSource ? CombatSource->DamageEffectClass : TargetASC->DamageEffectClass;
    if (!DamageEffect) return 0.0f;
    FGameplayEffectSpecHandle Spec = SpecASC->MakeOutgoingSpec(DamageEffect, 1.0f, Context);
    if (!Spec.IsValid()) return 0.0f;
    Spec.Data->SetSetByCallerMagnitude(CombatTags::Data_Damage, Damage);
    const float PreviousHealth = TargetASC->GetHealth();
    SpecASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
    return FMath::Max(0.0f, PreviousHealth - TargetASC->GetHealth());
}
