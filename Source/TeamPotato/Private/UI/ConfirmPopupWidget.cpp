// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ConfirmPopupWidget.h"
#include "Components/Button.h"

void UConfirmPopupWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (ConfirmButton)
    {
        ConfirmButton->OnClicked.AddDynamic(this, &UConfirmPopupWidget::OnConfirmButtonClicked);
    }
    if (CancelButton)
    {
        CancelButton->OnClicked.AddDynamic(this, &UConfirmPopupWidget::OnCancelButtonClicked);
    }
}

void UConfirmPopupWidget::NativeDestruct()
{
    if(ConfirmButton)
    {
        ConfirmButton->OnClicked.RemoveDynamic(this, &UConfirmPopupWidget::OnConfirmButtonClicked);
    }
    if(CancelButton)
    {
        CancelButton->OnClicked.RemoveDynamic(this, &UConfirmPopupWidget::OnCancelButtonClicked);
    }

    Super::NativeDestruct();
}

void UConfirmPopupWidget::OnConfirmButtonClicked()
{
    OnConfirmed.Broadcast();

    RemoveFromParent();
}

void UConfirmPopupWidget::OnCancelButtonClicked()
{
    RemoveFromParent();
}