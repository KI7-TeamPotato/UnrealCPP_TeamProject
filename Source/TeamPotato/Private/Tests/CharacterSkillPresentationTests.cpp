#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Data/CharacterDataTableRow.h"
#include "Engine/DataTable.h"
#include "Engine/Engine.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "Player/TestCharacter.h"
#include "UI/CharacterSelect/CharacterSelectDetailWidget.h"
#include "UObject/StrongObjectPtr.h"

namespace
{
    struct FSkillPresentationWorld
    {
        UWorld* World = nullptr;

        FSkillPresentationWorld()
        {
            World = UWorld::CreateWorld(EWorldType::GamePreview, false);
            GEngine->CreateNewWorldContext(EWorldType::GamePreview).SetCurrentWorld(World);
            World->InitializeActorsForPlay(FURL());
        }

        ~FSkillPresentationWorld()
        {
            World->DestroyWorld(false);
            GEngine->DestroyWorldContext(World);
        }
    };
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCharacterSkillPresentationTest, "TeamPotato.Character.SkillPresentationUpdatesOnSelection",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCharacterSkillPresentationTest::RunTest(const FString& Parameters)
{
    UClass* DetailClass = LoadClass<UCharacterSelectDetailWidget>(nullptr,
        TEXT("/Game/UI/Widget/PlayingGameUI/PlayerSelect/CharacterDetail/WBP_CharacterDetail.WBP_CharacterDetail_C"));
    UDataTable* Characters = LoadObject<UDataTable>(nullptr,
        TEXT("/Game/UI/Data/Character/DT_CharacterData.DT_CharacterData"));
    if (!TestNotNull(TEXT("Actual character detail widget loads"), DetailClass) ||
        !TestNotNull(TEXT("Actual character selection data loads"), Characters)) return false;

    FSkillPresentationWorld Fixture;
    TStrongObjectPtr<UCharacterSelectDetailWidget> Detail(CreateWidget<UCharacterSelectDetailWidget>(Fixture.World, DetailClass));
    if (!TestNotNull(TEXT("Character detail widget initializes its real bindings"), Detail.Get())) return false;
    auto* Icon = Cast<UImage>(Detail->GetWidgetFromName(TEXT("SkillIconImage")));
    auto* Name = Cast<UTextBlock>(Detail->GetWidgetFromName(TEXT("SkillNameText")));
    auto* Description = Cast<UTextBlock>(Detail->GetWidgetFromName(TEXT("SkillDescriptionText")));
    if (!TestNotNull(TEXT("Skill icon exists in the real widget tree"), Icon) ||
        !TestNotNull(TEXT("Skill name exists in the real widget tree"), Name) ||
        !TestNotNull(TEXT("Skill description exists in the real widget tree"), Description)) return false;

    TArray<FCharacterDataTableRow*> Rows;
    Characters->GetAllRows(TEXT("SkillPresentationTest"), Rows);
    if (!TestTrue(TEXT("Character selection includes all three playable characters"), Rows.Num() >= 3)) return false;
    for (const FCharacterDataTableRow* Row : Rows)
    {
        const ATestCharacter* Defaults = Row->CharacterClass.GetDefaultObject();
        if (!TestNotNull(TEXT("Selection row resolves its character Blueprint defaults"), Defaults)) return false;
        TestNotNull(TEXT("Character has a configured skill icon"), Defaults->GetSkillIcon());
        TestFalse(TEXT("Character has a configured skill name"), Defaults->GetSkillName().IsEmpty());
        TestFalse(TEXT("Character has a configured skill description"), Defaults->GetSkillDescription().IsEmpty());
        Detail->UpdateCharacterDetailsFromData(*Row);
        TestEqual(TEXT("Selecting another character replaces the skill icon"), Icon->GetBrush().GetResourceObject(),
            static_cast<UObject*>(Defaults->GetSkillIcon()));
        TestEqual(TEXT("Configured skill icon is visible"), Icon->GetVisibility(), ESlateVisibility::SelfHitTestInvisible);
        TestTrue(TEXT("Skill name follows selected character BP"), Name->GetText().EqualTo(Defaults->GetSkillName()));
        TestTrue(TEXT("Skill description follows selected character BP"), Description->GetText().EqualTo(Defaults->GetSkillDescription()));
    }

    // An incomplete row must not keep showing the previous character's skill.
    FCharacterDataTableRow MissingClass = *Rows[0];
    MissingClass.CharacterClass = nullptr;
    Detail->UpdateCharacterDetailsFromData(MissingClass);
    TestNull(TEXT("Missing character clears previous skill texture"), Icon->GetBrush().GetResourceObject());
    TestEqual(TEXT("Missing icon is hidden rather than rendered as a white image"), Icon->GetVisibility(), ESlateVisibility::Hidden);
    TestTrue(TEXT("Missing character clears previous skill name"), Name->GetText().IsEmpty());
    TestTrue(TEXT("Missing character clears previous description"), Description->GetText().IsEmpty());

    Detail->UpdateCharacterDetailsFromData(*Rows[0]);
    FCharacterDataTableRow EmptyPresentation = *Rows[0];
    EmptyPresentation.CharacterClass = ATestCharacter::StaticClass();
    Detail->UpdateCharacterDetailsFromData(EmptyPresentation);
    TestNull(TEXT("Unconfigured character clears previous skill texture"), Icon->GetBrush().GetResourceObject());
    TestTrue(TEXT("Unconfigured character clears previous text"), Name->GetText().IsEmpty() && Description->GetText().IsEmpty());
    Detail->UpdateCharacterDetailsFromData(*Rows[0]);
    TestEqual(TEXT("Selecting a configured character restores its icon"), Icon->GetVisibility(), ESlateVisibility::SelfHitTestInvisible);
    TestTrue(TEXT("Selecting again restores the correct description"),
        Description->GetText().EqualTo(Rows[0]->CharacterClass.GetDefaultObject()->GetSkillDescription()));
    return true;
}

#endif
