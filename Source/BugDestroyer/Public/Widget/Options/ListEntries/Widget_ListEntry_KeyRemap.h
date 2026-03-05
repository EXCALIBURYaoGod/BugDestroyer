// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget_ListEntry_Base.h"
#include "Widget_ListEntry_KeyRemap.generated.h"

class UListDataObject_KeyRemap;
class UBugCommonButtonBase;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class BUGDESTROYER_API UWidget_ListEntry_KeyRemap : public UWidget_ListEntry_Base
{
	GENERATED_BODY()
	
protected:
	//begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	//end UUserWidget Interface
	
	// begin UWidget_ListEntry_Base interface
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject) override;
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningListDataObject, EOptionsListDataModifyReason ModifyReason) override;
	// end UWidget_ListEntry_Base interface
	
private:
	void OnRemapKeyButtonClicked();
	void OnResetKeyBindingButtonClicked();
	
	void OnKeyToRemapPressed(const FKey& PressedKey);
	void OnKeyRemapCanceled(const FString& CanceledReason);
	
	//**** bound widgets ****//
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UBugCommonButtonBase* CommonButton_RemapKey;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UBugCommonButtonBase* CommonButton_ResetKeyBinding;
	//**** bound widgets ****//
	
	UPROPERTY(Transient)
	UListDataObject_KeyRemap* CachedOwningKeyRemapDataObject;
	
};
