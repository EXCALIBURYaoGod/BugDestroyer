// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BugTypes/BugEnumTypes.h"
#include "UObject/Object.h"
#include "ListDataObject_Base.generated.h"

#define LIST_DATA_ACCESSOR(DataType, PropertyName) \
	FORCEINLINE DataType Get##PropertyName() const {return PropertyName;} \
	void Set##PropertyName(DataType In##PropertyName) {PropertyName = In##PropertyName;}
struct FOptionsDataEditConditionDescriptor;
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnListDataModifiedDelegate, UListDataObject_Base*, EOptionsListDataModifyReason);
/**
 * 
 */
UCLASS(Abstract)
class BUGDESTROYER_API UListDataObject_Base : public UObject
{
	GENERATED_BODY()
	
public:
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnListDataModifiedDelegate, UListDataObject_Base*, EOptionsListDataModifyReason);
	FOnListDataModifiedDelegate OnListDataModifiedDelegate;
	FOnListDataModifiedDelegate OnDependencyDataModified;
	
	LIST_DATA_ACCESSOR(FName, DataID)
	LIST_DATA_ACCESSOR(FText, DataDisplayName)
	LIST_DATA_ACCESSOR(FText, DescriptionRichText)
	LIST_DATA_ACCESSOR(FText, DisabledRichText)
	LIST_DATA_ACCESSOR(TSoftObjectPtr<UTexture2D>, SoftDescriptionImage)
	LIST_DATA_ACCESSOR(UListDataObject_Base*, ParentData)
	
	void InitDataObject();
	
	/** Empty in the base class. Child Class ListDataObject_Collection should override it. 
	 The function shuould return all the child data a tab has */
	virtual TArray<UListDataObject_Base*> GetChildListDataObjects() const { return TArray<UListDataObject_Base*>(); }
	virtual bool HasAnyChildListDataObjects() const { return false; }
	
	void  SetShouldApplyChangeImmediately(bool InApplyChangeImmediately){ bShouldApplyChangeImmediately = InApplyChangeImmediately; }
	
	virtual bool HasDefaultValue() const {return false;}
	virtual bool CanResetBackToDefaultValue() const {return false;}
	virtual bool TryResetBackToDefaultValue() {return false;}
	
	// Gets called from OptionsDataRegistry for adding in edit conditions for the constructed list data object
	void AddEditCondition(const FOptionsDataEditConditionDescriptor& InEditCondition);
	
	void AddEditDependencyData(UListDataObject_Base* InDependencyData);
	
	bool IsDataCurrentlyEditable();
	
protected:
	// The child classes should override it to handle the initialization needed accordingly
	virtual void OnDataObjectInitialized();
	
	virtual void NotifyListDataModified(UListDataObject_Base* ModifiedData, 
		EOptionsListDataModifyReason ModifyReason = EOptionsListDataModifyReason::DirectlyModified);
	
	// The child class should override this to allow the value be set to the forced string value
	virtual bool CanSetToForcedStringValue(const FString& InForcedString) const{return false;}
	// The child class should override this to specify how to set the current value to the forced value
	virtual void OnSetToForcedStringValue(const FString& InForcedString){}
	// This function will be called when the value of the dependency data has changed. The child class can override this function to handle the custom logic needed. Super call is expected.
	virtual void OnEditDependencyDataModified(UListDataObject_Base* ModifiedDependencyData, EOptionsListDataModifyReason ModifyReason);
	
private:
	FName DataID;
	FText DataDisplayName;
	FText DescriptionRichText;
	FText DisabledRichText;
	TSoftObjectPtr<UTexture2D> SoftDescriptionImage;
	
	UPROPERTY(Transient)
	UListDataObject_Base* ParentData;
	
	bool bShouldApplyChangeImmediately = false;
	
	UPROPERTY(Transient)
	TArray<FOptionsDataEditConditionDescriptor> EditConditionDesArray;
	
};
