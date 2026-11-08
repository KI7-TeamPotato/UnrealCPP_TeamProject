// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterSelect/CharacterSelectDetailWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/GameInstance.h"
#include "Player/TestCharacter.h"
#include "Subsystem/CharacterSubsystem.h"

void UCharacterSelectDetailWidget::UpdateCharacterDetails(ECharacterType InCharacterType)
{
    UGameInstance* GameInstance = GetGameInstance();
    UCharacterSubsystem* Characters = GameInstance ? GameInstance->GetSubsystem<UCharacterSubsystem>() : nullptr;
    UpdateCharacterDetailsFromData(Characters ? Characters->GetCharacterDataRowByType(InCharacterType) : FCharacterDataTableRow());
}

void UCharacterSelectDetailWidget::UpdateCharacterDetailsFromData(const FCharacterDataTableRow& CharacterDataRow)
{
    if (CharacterTypeText) CharacterTypeText->SetText(FText::FromString(CharacterDataRow.CharacterName));
    if (CharacterHealthText) CharacterHealthText->SetText(FText::AsNumber(static_cast<int32>(CharacterDataRow.MaxHealth)));
    if (CharacterEnergyText) CharacterEnergyText->SetText(FText::AsNumber(static_cast<int32>(CharacterDataRow.MaxEnergy)));
    if (CharacterWalkSpeedText) CharacterWalkSpeedText->SetText(FText::AsNumber(static_cast<int32>(CharacterDataRow.WalkSpeed)));
    if (CharacterIllustImage) CharacterIllustImage->SetBrushFromTexture(CharacterDataRow.CharacterIllustration);

    const UWeaponDataAsset* Weapon = CharacterDataRow.DefaultWeaponDataAsset;
    if (WeaponIconImage) WeaponIconImage->SetBrushFromTexture(Weapon ? Weapon->WeaponIcon : nullptr);
    if (WeaponNameText) WeaponNameText->SetText(Weapon ? Weapon->WeaponName : FText::GetEmpty());
    if (WeaponDamageText) WeaponDamageText->SetText(Weapon ? FText::AsNumber(static_cast<int32>(Weapon->AttackDamage)) : FText::GetEmpty());
    if (WeaponAttackSpeedText) WeaponAttackSpeedText->SetText(Weapon ? FText::AsNumber(Weapon->AttackSpeed) : FText::GetEmpty());

    const ATestCharacter* CharacterDefaults = CharacterDataRow.CharacterClass.GetDefaultObject();
    UTexture2D* SkillIcon = CharacterDefaults ? CharacterDefaults->GetSkillIcon() : nullptr;
    if (SkillIconImage)
    {
        SkillIconImage->SetBrushFromTexture(SkillIcon);
        SkillIconImage->SetVisibility(SkillIcon ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Hidden);
    }
    if (SkillNameText) SkillNameText->SetText(CharacterDefaults ? CharacterDefaults->GetSkillName() : FText::GetEmpty());
    if (SkillDescriptionText) SkillDescriptionText->SetText(CharacterDefaults ? CharacterDefaults->GetSkillDescription() : FText::GetEmpty());
}
