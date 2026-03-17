// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_OptionsScreen.h"

#include "DebugHelper.h"
#include "ICommonInputModule.h"
#include "Chaos/ChaosPerfTest.h"
#include "Input/CommonUIInputTypes.h"
#include "Settings/BugGameUserSettings.h"
#include "Subsystems/BugUISubsystem.h"
#include "Widget/Components/BugCommonButtonBase.h"
#include "Widget/Components/BugCommonListView.h"
#include "Widget/Components/BugTabListWidgetBase.h"
#include "Widget/Options/OptionsDataRegistry.h"
#include "Widget/Options/Widget_OptionsDetailsView.h"
#include "Widget/Options/DataObjects/ListDataObjcet_Collection.h"
#include "Widget/Options/ListEntries/Widget_ListEntry_Base.h"


void UWidget_OptionsScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (!ResetAction.IsNull())
	{
		ResetActionHandle = RegisterUIActionBinding(
		FBindUIActionArgs(
			ResetAction,
			true,
			FSimpleDelegate::CreateUObject(this, &ThisClass::OnResetBoundActionTriggerred))
			);
	}

	TabListWidget_OptionsTabs->OnTabSelected.AddUniqueDynamic(this, &ThisClass::OnOptionsTabSelected);
	
	CommonListView_OptionsListView->OnItemIsHoveredChanged().AddUObject(this, &ThisClass::OnListViewItemHovered);
	CommonListView_OptionsListView->OnItemSelectionChanged().AddUObject(this, &ThisClass::OnListViewItemSelected);
	
}

void UWidget_OptionsScreen::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	UBugGameUserSettings::Get()->ApplySettings(true);
}

UWidget* UWidget_OptionsScreen::NativeGetDesiredFocusTarget() const
{
	if (UObject* SelectedObject = CommonListView_OptionsListView->GetSelectedItem())
	{
		if (UUserWidget* SelectedEntryWidget = CommonListView_OptionsListView->GetEntryWidgetFromItem(SelectedObject))
		{
			return SelectedEntryWidget;
		}
	}
	
	return Super::NativeGetDesiredFocusTarget();
}

void UWidget_OptionsScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	for ( auto TabCollection : GetOrCreateDataRegistry()->GetRegisteredOptionsTabCollections())
	{
		if (!TabCollection)
		{
			continue;
		}
		const FName TabID = TabCollection->GetDataID();
		if (TabListWidget_OptionsTabs->GetTabButtonBaseByID(TabID) != nullptr)
		{
			continue;
		}
		TabListWidget_OptionsTabs->RequestRegisterTab(TabID, TabCollection->GetDataDisplayName());
	}
}

UOptionsDataRegistry* UWidget_OptionsScreen::GetOrCreateDataRegistry()
{
	if (!CreatedOwningDataRegistry)
	{
		CreatedOwningDataRegistry = NewObject<UOptionsDataRegistry>();
		CreatedOwningDataRegistry->InitOptionsDataRegistry(GetOwningLocalPlayer());
	}
	
	checkf(CreatedOwningDataRegistry, TEXT("Data registry for options screen is not valid"));
	
	return CreatedOwningDataRegistry;
}

void UWidget_OptionsScreen::OnResetBoundActionTriggerred()
{
	if (ResettableDataArray.IsEmpty())
	{
		return;
	}
	
	UCommonButtonBase* SelectedTabButton = TabListWidget_OptionsTabs->GetTabButtonBaseByID(TabListWidget_OptionsTabs->GetActiveTab());
	const FString SelectedTabButtonName = CastChecked<UBugCommonButtonBase>(SelectedTabButton)->GetButtonDisplayText().ToString();
	
	UBugUISubsystem::Get(GetOwningPlayerController())->PushConfirmScreenToModalStackAsync(
		EConfirmScreenType::YesNo,
		FText::FromString(TEXT("Reset")),
		FText::FromString(TEXT("Are you sure you want to reset all the settings under the ") + 
			SelectedTabButtonName + TEXT("tab.")),
			[this](EConfirmScreenButtonType ClickedButtonType)
			{
				if (ClickedButtonType != EConfirmScreenButtonType::Closed)
				{
					return;
				}
				
				bIsResettingData = true;
				bool bHasDataFailedToReset = false;
				
				for (UListDataObject_Base* DataToReset : ResettableDataArray)
				{
					if (!DataToReset)
					{
						continue;
					}
					if (DataToReset->TryResetBackToDefaultValue())
					{
						Debug::Print(DataToReset->GetDataDisplayName().ToString() + TEXT("was reset"));
					}
					else
					{
						bHasDataFailedToReset = true;
						Debug::Print(DataToReset->GetDataDisplayName().ToString() + TEXT("failed to reset"));
					}
				}
				if (!bHasDataFailedToReset)
				{
					ResettableDataArray.Empty();
					RemoveActionBinding(ResetActionHandle);
				}
				
				bIsResettingData = false;
			}
		);
	
}

void UWidget_OptionsScreen::OnOptionsTabSelected(FName TabID)
{
	DetailsView_ListEntryInfo->ClearDetailsViewInfo();
	
	TArray<UListDataObject_Base*> FoundListSourceItems = GetOrCreateDataRegistry()->GetListSourceItemsBySelectedTabID(TabID);
	
	CommonListView_OptionsListView->SetListItems(FoundListSourceItems);
	CommonListView_OptionsListView->RequestRefresh();
	
	if (CommonListView_OptionsListView->GetNumItems() != 0)
	{
		CommonListView_OptionsListView->NavigateToIndex(0);
		CommonListView_OptionsListView->SetSelectedIndex(0);
	}
	
	ResettableDataArray.Empty();

	for (UListDataObject_Base* FoundListSourceItem : FoundListSourceItems)
	{
		if (!FoundListSourceItem)
		{
			continue;
		}
		if (!FoundListSourceItem->OnListDataModifiedDelegate.IsBoundToObject(this))
		{
			FoundListSourceItem->OnListDataModifiedDelegate.AddUObject(this, &ThisClass::OnListViewListDataModified);
		}
		
		if (FoundListSourceItem->CanResetBackToDefaultValue())
		{
			ResettableDataArray.AddUnique(FoundListSourceItem);
		}
	}

	if (ResettableDataArray.IsEmpty())
	{
		RemoveActionBinding(ResetActionHandle);
	}
	else
	{
		if (!GetActionBindings().Contains(ResetActionHandle))
		{
			AddActionBinding(ResetActionHandle);
		}
	}
	
}

void UWidget_OptionsScreen::OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered)
{
	if (!InHoveredItem)
	{
		return;
	}
	
	UWidget_ListEntry_Base* HoveredEntryWidget = 
		CommonListView_OptionsListView->GetEntryWidgetFromItem<UWidget_ListEntry_Base>(InHoveredItem);
	check(HoveredEntryWidget);
	HoveredEntryWidget->NativeOnListEntryWidgetHovered(bWasHovered);

	if (bWasHovered)
	{
		DetailsView_ListEntryInfo->UpdateDetailsViewInfo(
			CastChecked<UListDataObject_Base>(InHoveredItem),
			TryGetEntryWidgetClassName(InHoveredItem)
			);
	}
	else
	{
		if(UListDataObject_Base* SelectedItem = CommonListView_OptionsListView->GetSelectedItem<UListDataObject_Base>())
		{
			DetailsView_ListEntryInfo->UpdateDetailsViewInfo(
				SelectedItem,
				TryGetEntryWidgetClassName(SelectedItem)
				);
		}
	}
	
}

void UWidget_OptionsScreen::OnListViewItemSelected(UObject* InSelectedItem)
{
	if (!InSelectedItem)
	{
		return;
	}
	DetailsView_ListEntryInfo->UpdateDetailsViewInfo(
		CastChecked<UListDataObject_Base>(InSelectedItem),
		TryGetEntryWidgetClassName(InSelectedItem)
		);
	
}

FString UWidget_OptionsScreen::TryGetEntryWidgetClassName(UObject* InOwningListItem) const
{
	if (UUserWidget* FoundEntryWidget = CommonListView_OptionsListView->GetEntryWidgetFromItem(InOwningListItem))
	{
		return FoundEntryWidget->GetClass()->GetName();
	}
	
	return TEXT("Entry Widget Not Valid");
}

void UWidget_OptionsScreen::OnListViewListDataModified(UListDataObject_Base* ModifiedData,
	EOptionsListDataModifyReason ModifiedReason)
{
	if (!ModifiedData || bIsResettingData)
	{
		return;
	}
	if (ModifiedData->CanResetBackToDefaultValue())
	{
		ResettableDataArray.AddUnique(ModifiedData);

		if (!GetActionBindings().Contains(ResetActionHandle))
		{
			AddActionBinding(ResetActionHandle);
		}
	}
	else
	{
		if (ResettableDataArray.Contains(ModifiedData))
		{
			ResettableDataArray.Remove(ModifiedData);
		}
	}

	if (ResettableDataArray.IsEmpty())
	{
		RemoveActionBinding(ResetActionHandle);
	}
}
