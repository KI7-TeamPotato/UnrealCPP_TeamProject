// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterSelect/CharacterSelectMenu.h"
#include "Components/TileView.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Data/Object/CharacterDataObject.h"
#include "Subsystem/CharacterSubsystem.h"

void UCharacterSelectMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (CharacterTileView)
	{
		//서브 시스템에서 모든 캐릭터 정보 불러와 타일뷰에 아이템 오브젝트로 묶어서 추가
		LoadCharacterDataFromSubsystem();

		// 타일뷰의 OnItemClicked 델리게이트에 함수 바인딩, 타일뷰에서 아이템이 클릭되었을 때 호출되는 함수
		CharacterTileView->OnItemClicked().AddUObject(this, &UCharacterSelectMenu::OnCharacterPreviewTileSelected);
	}

	if (SelectButton)
	{
		SelectButton->OnClicked.AddDynamic(this, &UCharacterSelectMenu::OnSelectButtonClicked);
		SelectButton->SetIsEnabled(false); //초기에는 비활성화 => 캐릭터가 선택되면 활성화
	}
}

void UCharacterSelectMenu::NativeDestruct()
{
	if(CharacterTileView)
	{
		CharacterTileView->OnItemClicked().RemoveAll(this);
	}

	if (SelectButton)
	{
		SelectButton->OnClicked.RemoveDynamic(this, &UCharacterSelectMenu::OnSelectButtonClicked);
	}

	Super::NativeDestruct();
}

UCharacterSubsystem* UCharacterSelectMenu::GetCharacterSubsystem() const
{
	UGameInstance* GameInstance = Cast<UGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameInstance)
	{
		return GameInstance->GetSubsystem<UCharacterSubsystem>();
	}
	return nullptr;
}

// ====================================================================
// 서브시스템에서 캐릭터 정보 불러와 타일뷰에 아이템 오브젝트로 묶어서 추가
// ====================================================================
void UCharacterSelectMenu::LoadCharacterDataFromSubsystem()
{
	if (!CharacterTileView) return;

	// 플레이어 캐릭터 서브시스템에서 데이터 테이블 정보 가져오기
	UCharacterSubsystem* CharacterSubsystem = GetCharacterSubsystem();
	if (!CharacterSubsystem) return;
	
	const TMap<ECharacterType, FCharacterDataTableRow>& CharacterDataMap = CharacterSubsystem->GetAllCharacterDataMap();

	for (const auto& Pair : CharacterDataMap)
	{
		// 캐릭터 데이터 오브젝트 생성 & 데이터 주입
		UCharacterDataObject* CharacterDataObj = NewObject<UCharacterDataObject>(this);
		CharacterDataObj->InitFromDataTableRow(Pair.Value);

		// 타일뷰에 아이템으로 추가
		CharacterTileView->AddItem(CharacterDataObj);
	}
}

// ====================================================================================
// 타일뷰에서 아이템이 클릭되었을 때 그 아이템의 캐릭터 데이터 오브젝트를 가지고 실행되는 함수
// 캐릭터 데이터 오브젝트를 선택된 캐릭터 데이터로 저장하고, 일러스트 이미지를 설정
// ====================================================================================
void UCharacterSelectMenu::OnCharacterPreviewTileSelected(UObject* SelectedItem)
{
	UCharacterDataObject* CharacterDataObj = Cast<UCharacterDataObject>(SelectedItem);

	if (CharacterDataObj)
	{
		SelectedCharacterData = CharacterDataObj;

		SetCharacterIllustration(CharacterDataObj->CharacterIllustration);

		if (SelectButton)
		{
			SelectButton->SetIsEnabled(true); //캐릭터가 선택되었으므로 선택 버튼 활성화
		}
	}
}

// ========================================================================================
// 선택 버튼 클릭시에 선택된 캐릭터를 서브시스템에 설정 => 서브시스템에서 게임 전체에 브로드캐스트
// ========================================================================================
void UCharacterSelectMenu::OnSelectButtonClicked()
{
	if (!SelectedCharacterData) return;

	UCharacterSubsystem* CharacterSubsystem = GetCharacterSubsystem();
	if (!CharacterSubsystem) return;

	CharacterSubsystem->SetSelectedCharacterType(SelectedCharacterData->CharacterType);

	// UI 닫기 추가
    RemoveFromParent();
}

void UCharacterSelectMenu::SetCharacterIllustration(UTexture2D* InIllustration)
{
	if (CharacterImage && InIllustration)
	{
		CharacterImage->SetBrushFromTexture(InIllustration);
		CharacterImage->SetVisibility(ESlateVisibility::Visible);
	}
}

