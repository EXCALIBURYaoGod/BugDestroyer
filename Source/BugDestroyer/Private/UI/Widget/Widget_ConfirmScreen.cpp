// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_ConfirmScreen.h"

#include "CommonTextBlock.h"
#include "ICommonInputModule.h"
#include "Components/DynamicEntryBox.h"
#include "Widget/Components/BugCommonButtonBase.h"

UConfirmScreenInfoObject* UConfirmScreenInfoObject::CreateOkScreen(const FText& InScreenTitle,
                                                                   const FText& InScreenMessage)
{
	UConfirmScreenInfoObject* InfoObjcet = NewObject<UConfirmScreenInfoObject>();
	InfoObjcet->ScreenTitle = InScreenTitle;
	InfoObjcet->ScreenMessage = InScreenMessage;
	
	FConfirmScreenButtonInfo OkButtonInfo;
	OkButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Closed;
	OkButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("OK"));
	InfoObjcet->AvailableScreenButtonsInfos.Add(OkButtonInfo);
	
	return InfoObjcet;
}

UConfirmScreenInfoObject* UConfirmScreenInfoObject::CreateYesNoScreen(const FText& InScreenTitle,
	const FText& InScreenMessage)
{
	UConfirmScreenInfoObject* InfoObjcet = NewObject<UConfirmScreenInfoObject>();
	InfoObjcet->ScreenTitle = InScreenTitle;
	InfoObjcet->ScreenMessage = InScreenMessage;
	
	FConfirmScreenButtonInfo YesButtonInfo;
	YesButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Closed;
	YesButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("Yes"));
	InfoObjcet->AvailableScreenButtonsInfos.Add(YesButtonInfo);
	
	FConfirmScreenButtonInfo NoButtonInfo;
	NoButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Cancelled;
	NoButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("No"));
	InfoObjcet->AvailableScreenButtonsInfos.Add(NoButtonInfo);
	
	return InfoObjcet;
}

UConfirmScreenInfoObject* UConfirmScreenInfoObject::CreateOkCancelScreen(const FText& InScreenTitle,
	const FText& InScreenMessage)
{
	UConfirmScreenInfoObject* InfoObjcet = NewObject<UConfirmScreenInfoObject>();
	InfoObjcet->ScreenTitle = InScreenTitle;
	InfoObjcet->ScreenMessage = InScreenMessage;
	
	FConfirmScreenButtonInfo OkButtonInfo;
	OkButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Closed;
	OkButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("Ok"));
	InfoObjcet->AvailableScreenButtonsInfos.Add(OkButtonInfo);
	
	FConfirmScreenButtonInfo CancelButtonInfo;
	CancelButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Cancelled;
	CancelButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("Cancel"));
	InfoObjcet->AvailableScreenButtonsInfos.Add(CancelButtonInfo);
	
	return InfoObjcet;
}

void UWidget_ConfirmScreen::InitConfirmScreen(UConfirmScreenInfoObject* InScreenInfoObjcet,
	TFunction<void(EConfirmScreenButtonType)> ClickedButtonCallback)
{
	check(InScreenInfoObjcet && CommonTextBlock_Title && CommonTextBlock_Message);
	
	CommonTextBlock_Title->SetText(InScreenInfoObjcet->ScreenTitle);
	CommonTextBlock_Message->SetText(InScreenInfoObjcet->ScreenMessage);
	
	// 检查预先存在的旧按钮
	if (DynamicEntryBox_Buttons->GetNumEntries() != 0)
	{
		DynamicEntryBox_Buttons->Reset<UBugCommonButtonBase>(
			[](UBugCommonButtonBase& ExistingButton)
			{
				ExistingButton.OnClicked().Clear();
			}
		);
	}
	
	check(!InScreenInfoObjcet->AvailableScreenButtonsInfos.IsEmpty())

	for (const FConfirmScreenButtonInfo& AvailableButtonInfo : InScreenInfoObjcet->AvailableScreenButtonsInfos)
	{

		UBugCommonButtonBase* AddedButton = DynamicEntryBox_Buttons->CreateEntry<UBugCommonButtonBase>();
		AddedButton->SetButtonText(AvailableButtonInfo.ButtonTextToDisplay);
		
		AddedButton->OnClicked().AddLambda(
			[ClickedButtonCallback, AvailableButtonInfo, this]()
			{
				ClickedButtonCallback(AvailableButtonInfo.ConfirmScreenButtonType);
				
				DeactivateWidget();
			}
		);
	}
	
}

UWidget* UWidget_ConfirmScreen::NativeGetDesiredFocusTarget() const
{
	if (DynamicEntryBox_Buttons->GetNumEntries() != 0)
	{
		/**
		 *	Set focus on the last button. so if there are tow buttons, one is yes, one is no.
		 *	our gamepad will focus on the No button.
		 */
		DynamicEntryBox_Buttons->GetAllEntries().Last()->SetFocus();
	}
	
	
	return Super::NativeGetDesiredFocusTarget();
}
