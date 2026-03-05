// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Options/ListEntries/Widget_ListEntry_Base.h"

#include "CommonInputSubsystem.h"
#include "CommonTextBlock.h"
#include "DebugHelper.h"
#include "Components/ListView.h"
#include "Widget/Options/DataObjects/ListDataObject_Base.h"

FNavigationReply UWidget_ListEntry_Base::NativeOnNavigation(const FGeometry& MyGeometry,
                                                            const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
	return Super::NativeOnNavigation(MyGeometry, InNavigationEvent, InDefaultReply);
}

void UWidget_ListEntry_Base::NativeOnListEntryWidgetHovered(bool bWasHovered)
{
	BP_OnListEntryWidgetHovered(bWasHovered, GetListItem()? IsListItemSelected() : false);
	if (bWasHovered)
	{
		BP_OnToggleEntryWidgetHighlightState(bWasHovered);
	}
	else
	{
		BP_OnToggleEntryWidgetHighlightState(GetListItem() && IsListItemSelected());
	}
}

void UWidget_ListEntry_Base::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	OnOwningListDataObjectSet(CastChecked<UListDataObject_Base>(ListItemObject));
	
}

void UWidget_ListEntry_Base::NativeOnEntryReleased()
{
	IUserObjectListEntry::NativeOnEntryReleased();
	NativeOnListEntryWidgetHovered(false);
}

void UWidget_ListEntry_Base::NativeOnItemSelectionChanged(bool bIsSelected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);
	BP_OnToggleEntryWidgetHighlightState(bIsSelected);
}

FReply UWidget_ListEntry_Base::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem();
	if (CommonInputSubsystem && CommonInputSubsystem->GetCurrentInputType() == ECommonInputType::Gamepad)
	{
		if(UWidget* WidgetToFocus = BP_GetWidgetToFocusForGamepad())
		{
			if(TSharedPtr<SWidget> SlateWidgetToFocus = WidgetToFocus->GetCachedWidget())
			{
				return FReply::Handled().SetUserFocus(SlateWidgetToFocus.ToSharedRef());
			}
		}
	}

	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UWidget_ListEntry_Base::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
	if (CommonText_SettingDisplayName)
	{
		CommonText_SettingDisplayName->SetText(InOwningListDataObject->GetDataDisplayName());
	}
	
	if(!InOwningListDataObject->OnListDataModifiedDelegate.IsBoundToObject(this))
	{
		InOwningListDataObject->OnListDataModifiedDelegate.AddUObject(this, &ThisClass::OnOwningListDataObjectModified);
	}
	
	if(!InOwningListDataObject->OnDependencyDataModified.IsBoundToObject(this))
	{
		InOwningListDataObject->OnDependencyDataModified.AddUObject(this, &ThisClass::OnOwningDependencyDataModified);
	}
	
	OnToggleEditableState(InOwningListDataObject->IsDataCurrentlyEditable());
	
	CachedOwningDataObject = InOwningListDataObject;
}

void UWidget_ListEntry_Base::OnOwningListDataObjectModified(UListDataObject_Base* OwningListDataObject,
	EOptionsListDataModifyReason ModifyReason)
{
}

void UWidget_ListEntry_Base::OnOwningDependencyDataModified(UListDataObject_Base* OwningDependencyDataObject,
	EOptionsListDataModifyReason ModifyReason)
{
	if (CachedOwningDataObject)
	{
		OnToggleEditableState(CachedOwningDataObject->IsDataCurrentlyEditable());
	}
}

void UWidget_ListEntry_Base::OnToggleEditableState(bool bIsEditable)
{
	if (CommonText_SettingDisplayName)
	{
		CommonText_SettingDisplayName->SetIsEnabled(bIsEditable);
	}
}

void UWidget_ListEntry_Base::SelectThisEntryWidget()
{
	CastChecked<UListView>(GetOwningListView())->SetSelectedItem(GetListItem());
}
