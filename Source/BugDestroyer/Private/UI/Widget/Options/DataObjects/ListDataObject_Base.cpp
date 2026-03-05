// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Options/DataObjects/ListDataObject_Base.h"

#include "BugTypes/BugStructTypes.h"
#include "Settings/BugGameUserSettings.h"

void UListDataObject_Base::InitDataObject()
{
	OnDataObjectInitialized();
}

void UListDataObject_Base::AddEditCondition(const FOptionsDataEditConditionDescriptor& InEditCondition)
{
	EditConditionDesArray.Add(InEditCondition);
}

void UListDataObject_Base::AddEditDependencyData(UListDataObject_Base* InDependencyData)
{
	if (!InDependencyData->OnListDataModifiedDelegate.IsBoundToObject(this))
	{
		InDependencyData->OnListDataModifiedDelegate.AddUObject(this, &ThisClass::OnEditDependencyDataModified);
	}
}

bool UListDataObject_Base::IsDataCurrentlyEditable()
{
	bool bIsEditable = true;
	if (EditConditionDesArray.IsEmpty())
	{
		return bIsEditable;
	}

	FString CachedDisabledRichReason;
	
	for (const FOptionsDataEditConditionDescriptor& InEditCondition : EditConditionDesArray)
	{
		if (!InEditCondition.CheckEditConditionFuncIsValid() || InEditCondition.IsEditConditionMet())
		{
			continue;
		}
		bIsEditable = false;
		CachedDisabledRichReason.Append(InEditCondition.GetDisabledRichReason());
		
		SetDisabledRichText(FText::FromString(CachedDisabledRichReason));

		if (InEditCondition.HasForcedStringValue())
		{
			const FString ForcedStringValue = InEditCondition.GetDisabledForcedStringValue();
			// If the current value this dataobject has can be set to the forced value
			if (CanSetToForcedStringValue(ForcedStringValue))
			{
				OnSetToForcedStringValue(ForcedStringValue);
			}
			
		}
		
	}
	
	return bIsEditable;
	
}

void UListDataObject_Base::OnDataObjectInitialized()
{
	
}

void UListDataObject_Base::NotifyListDataModified(UListDataObject_Base* ModifiedData,
	EOptionsListDataModifyReason ModifyReason)
{
	OnListDataModifiedDelegate.Broadcast(ModifiedData, ModifyReason);

	if (bShouldApplyChangeImmediately)
	{
		UBugGameUserSettings::Get()->ApplySettings(true);
	}
}

void UListDataObject_Base::OnEditDependencyDataModified(UListDataObject_Base* ModifiedDependencyData,
	EOptionsListDataModifyReason ModifyReason)
{
	OnDependencyDataModified.Broadcast(ModifiedDependencyData, EOptionsListDataModifyReason::DependencyModified);
}
