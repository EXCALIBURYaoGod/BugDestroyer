// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Components/BugCommonButtonBase.h"

#include "CommonLazyImage.h"
#include "CommonTextBlock.h"
#include "UI/Subsystems/BugUISubsystem.h"

void UBugCommonButtonBase::SetButtonText(FText InButtonText)
{
	if (CommonTextBlock_ButtonText && !InButtonText.IsEmpty())
	{
		CommonTextBlock_ButtonText->SetText(bUseUpperCaseForButtonText? InButtonText.ToUpper() : InButtonText);
	}
}

FText UBugCommonButtonBase::GetButtonDisplayText()
{
	if (CommonTextBlock_ButtonText)
	{
		return CommonTextBlock_ButtonText->GetText();
	}
	return FText();
}

void UBugCommonButtonBase::SetButtonDisplayImage(const FSlateBrush& InButtonImage)
{
	if (CommonLazyImage_ButtonImage)
	{
		CommonLazyImage_ButtonImage->SetBrush(InButtonImage);
	}
}

void UBugCommonButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	SetButtonText(ButtonDisplayText);
}

void UBugCommonButtonBase::NativeOnCurrentTextStyleChanged()
{
	Super::NativeOnCurrentTextStyleChanged();
	if (CommonTextBlock_ButtonText && GetCurrentTextStyleClass())
	{
		CommonTextBlock_ButtonText->SetStyle(GetCurrentTextStyleClass());
	}
}

void UBugCommonButtonBase::NativeOnHovered()
{
	Super::NativeOnHovered();
	if (!ButtonDescriptionText.IsEmpty())
	{
		UBugUISubsystem::Get(GetOwningPlayer())->OnButtonDescriptionTextUpdatedDelegate.Broadcast(
			this, ButtonDescriptionText);
	}
}

void UBugCommonButtonBase::NativeOnUnhovered()
{
	Super::NativeOnUnhovered();

	UBugUISubsystem::Get(GetOwningPlayer())->OnButtonDescriptionTextUpdatedDelegate.Broadcast(
	this, FText::GetEmpty());
}
