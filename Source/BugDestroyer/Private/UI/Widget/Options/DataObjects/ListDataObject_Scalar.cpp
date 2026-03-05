// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Options/DataObjects/ListDataObject_Scalar.h"

#include "DebugHelper.h"
#include "GameFramework/GameUserSettings.h"

FCommonNumberFormattingOptions UListDataObject_Scalar::NoDecimal()
{
	FCommonNumberFormattingOptions Options;
	Options.MaximumFractionalDigits = 0;
	return Options;
}

FCommonNumberFormattingOptions UListDataObject_Scalar::WithDecimal(int32 NumFracDigit)
{
	FCommonNumberFormattingOptions Options;
	Options.MaximumFractionalDigits = NumFracDigit;
	return Options;
}

float UListDataObject_Scalar::GetCurrentValue() const
{
	if (DataDynamicGetter)
	{
		return FMath::GetMappedRangeValueClamped(
			OutputValueRange,
			DisplayValueRange,
			StringToFloat(DataDynamicGetter->GetValueAsString())
				);
	}
	return 0.0f;
}

void UListDataObject_Scalar::SetCurrentValueFromSliderValue(float CurrentValue)
{
	if (DataDynamicSetter)
	{
		const float ClampedValue = FMath::GetMappedRangeValueClamped(
				DisplayValueRange,
				OutputValueRange,
				CurrentValue
			);
		DataDynamicSetter->SetValueFromString(LexToString(ClampedValue));
		NotifyListDataModified(this);
	}
}


bool UListDataObject_Scalar::CanResetBackToDefaultValue() const
{
	if (HasDefaultValue() && DataDynamicGetter) 
	{
		const float DefaultValue = StringToFloat(GetDefaultValueAsString());
		const float CurrentValue = StringToFloat(DataDynamicGetter->GetValueAsString());
		
		return !FMath::IsNearlyEqual(CurrentValue, DefaultValue, 0.01f);
	}
	
	return Super::CanResetBackToDefaultValue();
}

bool UListDataObject_Scalar::TryResetBackToDefaultValue()
{
	if (CanResetBackToDefaultValue())
	{
		if (DataDynamicSetter)
		{
			DataDynamicSetter->SetValueFromString(GetDefaultValueAsString());
			NotifyListDataModified(this, EOptionsListDataModifyReason::ResetToDefault);
			return true;
		}
	}
	return Super::TryResetBackToDefaultValue();
}

void UListDataObject_Scalar::OnEditDependencyDataModified(UListDataObject_Base* ModifiedDependencyData,
	EOptionsListDataModifyReason ModifyReason)
{
	NotifyListDataModified(this, EOptionsListDataModifyReason::DependencyModified);
	Super::OnEditDependencyDataModified(ModifiedDependencyData, ModifyReason);
}

float UListDataObject_Scalar::StringToFloat(const FString& InString) const
{
	float OutConvertedValue = 0.f;
	LexFromString(OutConvertedValue, *InString);
	return OutConvertedValue;
}



