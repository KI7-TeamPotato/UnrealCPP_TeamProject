// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterSelect/CharacterSelectDetailWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Subsystem/CharacterSubsystem.h"

void UCharacterSelectDetailWidget::UpdateCharacterDetails(ECharacterType InCharacterType)
{
    const FCharacterDataTableRow& CharacterDataRow = GetWorld()->GetGameInstance()->GetSubsystem<UCharacterSubsystem>()->GetCharacterDataRowByType(InCharacterType);

    CharacterTypeText->SetText(FText::FromString(CharacterDataRow.CharacterName));
    CharacterHealthText->SetText(FText::AsNumber(static_cast<int32>(CharacterDataRow.MaxHealth)));
    CharacterEnergyText->SetText(FText::AsNumber(static_cast<int32>(CharacterDataRow.MaxEnergy)));
    CharacterWalkSpeedText->SetText(FText::AsNumber(static_cast<int32>(CharacterDataRow.WalkSpeed)));
    WeaponIconImage->SetBrushFromTexture(CharacterDataRow.DefaultWeaponDataAsset->WeaponIcon);
    WeaponNameText->SetText(CharacterDataRow.DefaultWeaponDataAsset->WeaponName);
    WeaponDamageText->SetText(FText::AsNumber(static_cast<int32>(CharacterDataRow.DefaultWeaponDataAsset->AttackDamage)));
    CharacterIllustImage->SetBrushFromTexture(CharacterDataRow.CharacterIllustration);
}
