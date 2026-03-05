// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget_ListEntry_Base.h"
#include "Widget_ListEntry_String.generated.h"

class UListDataObject_String;
class UBugCommonRotator;
class UBugCommonButtonBase;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class BUGDESTROYER_API UWidget_ListEntry_String : public UWidget_ListEntry_Base
{
	GENERATED_BODY()
	
protected:
	//begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	//end UUserWidget Interface
	
	// begin UWidget_ListEntry_Base interface
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject) override;
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningListDataObject, 
		EOptionsListDataModifyReason ModifyReason) override;
	virtual void OnToggleEditableState(bool bIsEditable) override;
	// end UWidget_ListEntry_Base interface
	
private:
	void OnPreviousOptionButtonClicked();
	void OnNextOptionButtonClicked();
	void OnRotatorValueChanged(int32 Value, bool bUserInitiated);
	
	
	//**** bound widgets ****//
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UBugCommonButtonBase* CommonButton_PreviousOption;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UBugCommonButtonBase* CommonButton_NextOption;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UBugCommonRotator* CommonRotator_AvailableOptions;
	//**** bound widgets ****//
	
	UPROPERTY(Transient)
	UListDataObject_String* CachedOwningStringDataObject;
	 
};
