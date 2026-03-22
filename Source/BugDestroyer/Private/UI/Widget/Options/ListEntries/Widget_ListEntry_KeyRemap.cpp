// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Options/ListEntries/Widget_ListEntry_KeyRemap.h"

#include "BugGameplayTags.h"
#include "BugUIFunctionLibrary.h"
#include "UI/Subsystems/BugUISubsystem.h"
#include "Widget/Widget_KeyRemapScreen.h"
#include "Widget/Components/BugCommonButtonBase.h"
#include "Widget/Options/DataObjects/ListDataObject_KeyRemap.h"

void UWidget_ListEntry_KeyRemap::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	CommonButton_RemapKey->OnClicked().AddUObject(this, &ThisClass::OnRemapKeyButtonClicked);
	CommonButton_ResetKeyBinding->OnClicked().AddUObject(this, &ThisClass::OnResetKeyBindingButtonClicked);
}

void UWidget_ListEntry_KeyRemap::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);
	
	CachedOwningKeyRemapDataObject = CastChecked<UListDataObject_KeyRemap>(InOwningListDataObject);
	
	CommonButton_RemapKey->SetButtonDisplayImage(CachedOwningKeyRemapDataObject->GetIconFromCurrentKey());
	CachedOwningKeyRemapDataObject->OnKeyBindConflict.AddUniqueDynamic(this, &ThisClass::HandleKeyBindConflict);
	
}

void UWidget_ListEntry_KeyRemap::OnOwningListDataObjectModified(UListDataObject_Base* OwningListDataObject,
	EOptionsListDataModifyReason ModifyReason)
{
	Super::OnOwningListDataObjectModified(OwningListDataObject, ModifyReason);
	if (CachedOwningKeyRemapDataObject)
	{
		CommonButton_RemapKey->SetButtonDisplayImage(CachedOwningKeyRemapDataObject->GetIconFromCurrentKey());
	}
}

void UWidget_ListEntry_KeyRemap::HandleKeyBindConflict(FName ConflictingActionName, FKey AttemptedKey)
{
	// 构造友好的提示文案
	FText Title = FText::FromString(TEXT("Key Conflict Warning"));
	FText Message = FText::Format(
		FText::FromString(TEXT("The key '{0}' is already bound to the action '{1}'.\nDo you want to unbind it and assign it to '{2}'?")),
		AttemptedKey.GetDisplayName(),
		FText::FromName(ConflictingActionName), // 提示被抢的动作名
		CachedOwningKeyRemapDataObject->GetDataDisplayName() // 提示当前正在改的动作名
	);

	// 弹出 Yes/No 对话框
	UBugUISubsystem::Get(GetOwningPlayer())->PushConfirmScreenToModalStackAsync(
		EConfirmScreenType::YesNo,
		Title,
		Message,
		[this, ConflictingActionName, AttemptedKey](EConfirmScreenButtonType ClickedButton)
		{
			if (ClickedButton == EConfirmScreenButtonType::Closed)
			{
				// 玩家点击 Yes，确认抢夺按键！
				if (CachedOwningKeyRemapDataObject)
				{
					CachedOwningKeyRemapDataObject->ForceBindInputKey(ConflictingActionName, AttemptedKey);
				}
			}
			else
			{
				// 玩家点击 No 或关闭了窗口，不做任何处理。
				// 此时按键保持原样，保护了玩家的原有设置。
			}
		}
	);
}

void UWidget_ListEntry_KeyRemap::OnRemapKeyButtonClicked()
{
	SelectThisEntryWidget();
	
	UBugUISubsystem::Get(GetOwningPlayer())->PushSoftWidgetToStackAsync(
		BugGameplayTags::Bug_WidgetStack_Modal,
		UBugUIFunctionLibrary::GetSoftWidgetClassByTag(BugGameplayTags::Bug_Widget_KeyRemapScreen),
		[this](EAsyncPushWidgetState PushWidgetState, UWidget_ActivatableBase* PushedWidget)
		{
			if (PushWidgetState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				UWidget_KeyRemapScreen* CreatedKeyRemapScreen = CastChecked<UWidget_KeyRemapScreen>(PushedWidget);
				CreatedKeyRemapScreen->OnKeyRemapScreenKeyPressed.BindUObject(this, &ThisClass::OnKeyToRemapPressed);
				CreatedKeyRemapScreen->OnKeyRemapScreenKeySelectedCanceled.BindUObject(this, &ThisClass::OnKeyRemapCanceled);
				if (CachedOwningKeyRemapDataObject)
				{
					CreatedKeyRemapScreen->SetDesiredInputTypeToFilter(CachedOwningKeyRemapDataObject->GetDesiredInputType());
				}
				
			}
		}
	);
}

void UWidget_ListEntry_KeyRemap::OnResetKeyBindingButtonClicked()
{
	SelectThisEntryWidget();
	if (!CachedOwningKeyRemapDataObject)
	{
		return;
	}

	if (CachedOwningKeyRemapDataObject->CanResetBackToDefaultValue())
	{
		UBugUISubsystem::Get(GetOwningPlayer())->PushConfirmScreenToModalStackAsync(
			EConfirmScreenType::YesNo,
			FText::FromString(TEXT("Reset Key Mapping")),
			FText::FromString(TEXT("Are you sure you want to reset the key binding for") + 
				CachedOwningKeyRemapDataObject->GetDataDisplayName().ToString() + TEXT(" ?")),
			[this](EConfirmScreenButtonType ClickedButton)
					{
						if (ClickedButton == EConfirmScreenButtonType::Closed)
						{
							CachedOwningKeyRemapDataObject->TryResetBackToDefaultValue();
						}
					}
			);
	}
	
}

void UWidget_ListEntry_KeyRemap::OnKeyToRemapPressed(const FKey& PressedKey)
{
	if (CachedOwningKeyRemapDataObject)
	{
		CachedOwningKeyRemapDataObject->BindNewInputKey(PressedKey);
	}
}

void UWidget_ListEntry_KeyRemap::OnKeyRemapCanceled(const FString& CanceledReason)
{
	UBugUISubsystem::Get(GetOwningPlayer())->PushConfirmScreenToModalStackAsync(
			EConfirmScreenType::Ok,
			FText::FromString(TEXT("Key Remap")),
			FText::FromString(CanceledReason),
			[](EConfirmScreenButtonType ClickedButton){}
		);
}
