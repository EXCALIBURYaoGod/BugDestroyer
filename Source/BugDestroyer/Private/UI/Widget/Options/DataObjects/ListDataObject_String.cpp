// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Options/DataObjects/ListDataObject_String.h"

#include "AudioMixerBlueprintLibrary.h"
#include "DebugHelper.h"
#include "GameFramework/GameUserSettings.h"

void UListDataObject_String::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();
	if (!AvailableOptionsStrings.IsEmpty())
	{
		CurrentStringValue = AvailableOptionsStrings[0];
	}

	if (HasDefaultValue())
	{
		CurrentStringValue = GetDefaultValueAsString();
	}
	
	if (DataDynamicGetter)
	{
		if (!DataDynamicGetter->GetValueAsString().IsEmpty())
		{
			CurrentStringValue = DataDynamicGetter->GetValueAsString();
		}
	}
	
	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentDisplayText = FText::FromString(TEXT("Invalid Option"));
	}
}

void UListDataObject_String::AddDynamicOption(const FString& InStringValue, const FText& InDisplayText)
{
	AvailableOptionsStrings.Add(InStringValue);
	AvailableOptionsTexts.Add(InDisplayText);
}

void UListDataObject_String::AdvanceToNextOption()
{
	if (AvailableOptionsStrings.IsEmpty() || AvailableOptionsTexts.IsEmpty())
	{
		return;
	}
	
	const int32 CurrentDisplayIndex = AvailableOptionsStrings.IndexOfByKey(CurrentStringValue);
	const int32 NextIndexToDisplay = CurrentDisplayIndex + 1;
	
	const bool bIsNextIndexValid = AvailableOptionsStrings.IsValidIndex(NextIndexToDisplay);
	if (bIsNextIndexValid)
	{
		CurrentStringValue = AvailableOptionsStrings[NextIndexToDisplay];
	}
	else
	{
		CurrentStringValue = AvailableOptionsStrings[0];
	}
	
	TrySetDisplayTextFromStringValue(CurrentStringValue);
	if (DataDynamicSetter)
	{
		DataDynamicSetter->SetValueFromString(CurrentStringValue);
		NotifyListDataModified(this);
	}
	
	
}

void UListDataObject_String::BackToPreviousOption()
{
	if (AvailableOptionsStrings.IsEmpty() || AvailableOptionsTexts.IsEmpty())
	{
		return;
	}
	
	const int32 CurrentDisplayIndex = AvailableOptionsStrings.IndexOfByKey(CurrentStringValue);
	const int32 PreviousIndexToDisplay = CurrentDisplayIndex - 1;
	
	const bool bIsPreviousIndexValid = AvailableOptionsStrings.IsValidIndex(PreviousIndexToDisplay);
	if (bIsPreviousIndexValid)
	{
		CurrentStringValue = AvailableOptionsStrings[PreviousIndexToDisplay];
	}
	else
	{
		CurrentStringValue = AvailableOptionsStrings.Last();
	}
	
	TrySetDisplayTextFromStringValue(CurrentStringValue);
	if (DataDynamicSetter)
	{
		DataDynamicSetter->SetValueFromString(CurrentStringValue);
		NotifyListDataModified(this);
	}
	
}

void UListDataObject_String::OnRotatorInitiatedValueChange(const FText& InNewSelectedText)
{
	const int32 FoundIndex = AvailableOptionsTexts.IndexOfByPredicate(
		[InNewSelectedText](const FText& AvailableText)->bool
		{
			return AvailableText.EqualTo(InNewSelectedText);
		}
	);

	if (FoundIndex != INDEX_NONE && AvailableOptionsStrings.IsValidIndex(FoundIndex))
	{
		CurrentDisplayText = InNewSelectedText;
		CurrentStringValue = AvailableOptionsStrings[FoundIndex];

		if (DataDynamicSetter)
		{
			DataDynamicSetter->SetValueFromString(CurrentStringValue);
			NotifyListDataModified(this);
		}
	}
	
}


bool UListDataObject_String::CanResetBackToDefaultValue() const
{
	return HasDefaultValue() && CurrentStringValue != GetDefaultValueAsString();
}

bool UListDataObject_String::TryResetBackToDefaultValue()
{
	if (CanResetBackToDefaultValue())
	{
		CurrentStringValue = GetDefaultValueAsString();
		
		TrySetDisplayTextFromStringValue(CurrentStringValue);

		if (DataDynamicSetter)
		{
			DataDynamicSetter->SetValueFromString(CurrentStringValue);
			NotifyListDataModified(this, EOptionsListDataModifyReason::ResetToDefault);
			return true;
		}
	}

	return false;
}

bool UListDataObject_String::CanSetToForcedStringValue(const FString& InForcedString) const
{
	return CurrentStringValue != InForcedString;
}

void UListDataObject_String::OnSetToForcedStringValue(const FString& InForcedString)
{
	CurrentStringValue = InForcedString;
	TrySetDisplayTextFromStringValue(CurrentStringValue);

	if (DataDynamicSetter)
	{
		DataDynamicSetter->SetValueFromString(CurrentStringValue);
		NotifyListDataModified(this, EOptionsListDataModifyReason::DependencyModified);
	}
}

bool UListDataObject_String::TrySetDisplayTextFromStringValue(const FString& InStringValue)
{
	const int32 CurrentFoundIndex = AvailableOptionsStrings.IndexOfByKey(InStringValue);
	if (AvailableOptionsTexts.IsValidIndex(CurrentFoundIndex))
	{
		CurrentDisplayText = AvailableOptionsTexts[CurrentFoundIndex];
		return true;
	}
	return false;
}

//****  UListDataObject_StringBool ****//

void UListDataObject_StringBool::OverrideTrueDisplayText(const FText& InNewTrueDisplayText)
{
	if (!AvailableOptionsStrings.Contains(TrueString))
	{
		AddDynamicOption(TrueString, InNewTrueDisplayText);
	}
}

void UListDataObject_StringBool::OverrideFalseDisplayText(const FText& InNewFalseDisplayText)
{
	if (!AvailableOptionsStrings.Contains(FalseString))
	{
		AddDynamicOption(FalseString, InNewFalseDisplayText);
	}
}

void UListDataObject_StringBool::SetTrueAsDefaultValue()
{
	SetDefaultValueFromString(TrueString);
}

void UListDataObject_StringBool::SetFalseAsDefaultValue()
{
	SetDefaultValueFromString(FalseString);
}

void UListDataObject_StringBool::OnDataObjectInitialized()
{
	TryInitBoolValues();
	Super::OnDataObjectInitialized();
	
}

void UListDataObject_StringBool::TryInitBoolValues()
{
	if (!AvailableOptionsStrings.Contains(TrueString))
	{
		AddDynamicOption(TrueString, FText::FromString(TEXT("ON")));
	}
	
	if (!AvailableOptionsStrings.Contains(FalseString))
	{
		AddDynamicOption(FalseString, FText::FromString(TEXT("OFF")));
	}
}


//****  UListDataObject_StringBool ****//

//****  UListDataObject_StringInteger ****//

void UListDataObject_StringInteger::AddIntegerOption(int32 InIntegerValue, const FText& InDisplayText)
{
	AddDynamicOption(LexToString(InIntegerValue), InDisplayText);
}


void UListDataObject_StringInteger::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();
	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentDisplayText = FText::FromString(TEXT("Custom"));
	}
	
}

void UListDataObject_StringInteger::OnEditDependencyDataModified(UListDataObject_Base* ModifiedDependencyData,
	EOptionsListDataModifyReason ModifyReason)
{
	if (DataDynamicGetter)
	{
		if (CurrentStringValue == DataDynamicGetter->GetValueAsString())
		{
			return;
		}
		
		CurrentStringValue = DataDynamicGetter->GetValueAsString();
		if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
		{
			CurrentDisplayText = FText::FromString(TEXT("Custom"));
		}
		
		NotifyListDataModified(this, ModifyReason);
	}
	Super::OnEditDependencyDataModified(ModifiedDependencyData, ModifyReason);
}

//****  UListDataObject_StringInteger ****//
