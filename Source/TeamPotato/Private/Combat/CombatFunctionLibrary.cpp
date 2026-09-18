#include "Combat/CombatFunctionLibrary.h"
#include "Combat/CombatAbilitySystemComponent.h"
#include "Combat/CombatTags.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

namespace
{
    UAbilitySystemComponent* FindSourceASC(AActor* Causer, AController* Controller)
    {
        AActor* Source = IsValid(Controller) ? Controller->GetPawn() : nullptr;
        if (!Source && IsValid(Causer)) Source = Causer->GetInstigator();
        if (!Source) Source = Causer;
        TSet<AActor*> Visited;
        while (IsValid(Source) && !Visited.Contains(Source))
        {
            Visited.Add(Source);
            if (auto* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Source, true)) return ASC;
            Source = Source->GetOwner();
        }
        return nullptr;
    }

    bool ApplyStatusInternal(AActor* Target, UCombatStatusDefinition* Definition, AActor* Causer, AController* Controller, bool bAcceptedHit)
    {
        if (!IsValid(Target) || !Target->HasAuthority() || !Target->CanBeDamaged()) return false;
        auto* ASC = Cast<UCombatAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Target, true));
        if (!ASC || (!bAcceptedHit && ASC->HasMatchingGameplayTag(CombatTags::State_Invincible))) return false;
        UAbilitySystemComponent* Source = FindSourceASC(Causer, Controller);
        FGameplayEffectContextHandle Context = (Source ? Source : ASC)->MakeEffectContext();
        Context.AddInstigator(Source ? Source->GetAvatarActor() : Causer, Causer);
        if (IsValid(Causer)) Context.AddSourceObject(Causer);
        return ASC->ApplyStatus(Definition, Context);
    }
}

bool UCombatFunctionLibrary::CanActorAttack(AActor* Actor)
{
    if (!IsValid(Actor)) return false;
    const auto* ASC = Cast<UCombatAbilitySystemComponent>(FindSourceASC(Actor, nullptr));
    return !ASC || ASC->CanAttack();
}

bool UCombatFunctionLibrary::ApplyCombatStatus(AActor* Target, UCombatStatusDefinition* Definition, AActor* DamageCauser, AController* InstigatorController)
{
    return ApplyStatusInternal(Target, Definition, DamageCauser, InstigatorController, false);
}

float UCombatFunctionLibrary::ApplyCombatDamageWithStatuses(AActor* Target, float Damage, AActor* DamageCauser,
    const TArray<UCombatStatusDefinition*>& Statuses, AController* InstigatorController)
{
    return ApplyCombatHitWithStatuses(Target, Damage, DamageCauser, Statuses, InstigatorController, nullptr);
}

float UCombatFunctionLibrary::ApplyCombatHitWithStatuses(AActor* Target, float Damage, AActor* DamageCauser,
    const TArray<UCombatStatusDefinition*>& Statuses, AController* InstigatorController, const FHitResult* Hit, TSubclassOf<UDamageType> DamageType)
{
    const float Applied = ApplyCombatDamageWithHit(Target, Damage, DamageCauser, InstigatorController, Hit, DamageType);
    if (Applied > 0.0f)
        for (UCombatStatusDefinition* Definition : Statuses)
            ApplyStatusInternal(Target, Definition, DamageCauser, InstigatorController, true);
    return Applied;
}

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
    // Also cover legacy BP ApplyDamage from characters/attached melee weapons. Detached,
    // already-fired projectiles are intentionally allowed to finish after their owner is shocked.
    if (const auto* CombatAttacker = Cast<UCombatAbilitySystemComponent>(SourceASC))
    {
        AActor* Avatar = CombatAttacker->GetAvatarActor();
        if (CombatAttacker->CanReceiveStatuses() && IsValid(DamageCauser) && IsValid(Avatar) &&
            (DamageCauser == Avatar || DamageCauser->IsAttachedTo(Avatar)) && !CombatAttacker->CanAttack()) return 0.0f;
    }
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
