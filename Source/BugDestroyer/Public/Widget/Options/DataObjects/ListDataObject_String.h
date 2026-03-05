// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ListDataObject_Value.h"
#include "ListDataObject_String.generated.h"

/**
 * 
 */
UCLASS()
class BUGDESTROYER_API UListDataObject_String : public UListDataObject_Value
{
	GENERATED_BODY()
	
public:
	void AddDynamicOption(const FString& InStringValue, const FText& InDisplayText);
	void AdvanceToNextOption();
	void BackToPreviousOption();
	void OnRotatorInitiatedValueChange(const FText& InNewSelectedText);
	
protected:
	//begin UListDataObject_Base Interface
	virtual void OnDataObjectInitialized() override;
	virtual bool CanResetBackToDefaultValue() const override;
	virtual bool TryResetBackToDefaultValue() override;
	virtual bool CanSetToForcedStringValue(const FString& InForcedString) const override;
	virtual void OnSetToForcedStringValue(const FString& InForcedString) override;
	//end UListDataObject_Base Interface
	
	bool TrySetDisplayTextFromStringValue(const FString& InStringValue);
	
	FString CurrentStringValue;
	FText CurrentDisplayText;
	TArray<FString> AvailableOptionsStrings;
	TArray<FText> AvailableOptionsTexts;
	
public:
	FORCEINLINE const TArray<FText>& GetAvailableOptionsTexts() const { return AvailableOptionsTexts; }
	FORCEINLINE FText GetCurrentDisplayText() const { return CurrentDisplayText; }
};

//****  UListDataObject_StringBool ****//

UCLASS()
class BUGDESTROYER_API UListDataObject_StringBool : public UListDataObject_String
{
	GENERATED_BODY()
	
public:
	void OverrideTrueDisplayText(const FText& InNewTrueDisplayText);
	void OverrideFalseDisplayText(const FText& InNewFalseDisplayText);
	void SetTrueAsDefaultValue();
	void SetFalseAsDefaultValue();
	
protected:
	//begin UListDataObject_Base Interface
	virtual void OnDataObjectInitialized() override;
	//begin UListDataObject_Base Interface
	
private:
	void TryInitBoolValues();
	
	const FString TrueString = TEXT("ture");
	const FString FalseString = TEXT("false");
	
};

//****  UListDataObject_StringBool ****//

//****  UListDataObject_StringEnum ****//

UCLASS()
class BUGDESTROYER_API UListDataObject_StringEnum : public UListDataObject_String
{
	GENERATED_BODY()
	
public:
	template <typename EnumType>
	void AddEnumOption(EnumType InEnumOption, const FText& InDisplayText)
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();
		const FString ConvertedEnumString = StaticEnumOption->GetNameStringByValue(InEnumOption);
		
		AddDynamicOption(ConvertedEnumString, InDisplayText);
	}
	
	template <typename EnumType>
	EnumType GetCurrentValueAsEnum() const
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();
		
		return (EnumType)StaticEnumOption->GetValueByNameString(CurrentStringValue);
	}
	
	template <typename EnumType>
	void SetDefaultValueFromEnumOption(EnumType InEnumOption)
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();
		const FString ConvertedEnumString = StaticEnumOption->GetNameStringByValue(InEnumOption);
		
		SetDefaultValueFromString(ConvertedEnumString);
	}

	
};

//****  UListDataObject_StringEnum ****//

//****  UListDataObject_StringInteger ****//

UCLASS()
class BUGDESTROYER_API UListDataObject_StringInteger : public UListDataObject_String
{
	GENERATED_BODY()
	
public:
	void AddIntegerOption(int32 InIntegerValue, const FText& InDisplayText);
	
protected:
	//begin UListDataObject_Base Interface
	virtual void OnDataObjectInitialized() override;
	virtual void OnEditDependencyDataModified(UListDataObject_Base* ModifiedDependencyData, EOptionsListDataModifyReason ModifyReason) override;
	//begin UListDataObject_Base Interface
	
};

//****  UListDataObject_StringInteger ****//