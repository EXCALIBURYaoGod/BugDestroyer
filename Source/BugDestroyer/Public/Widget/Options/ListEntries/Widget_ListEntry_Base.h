// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "BugTypes/BugEnumTypes.h"
#include "Widget_ListEntry_Base.generated.h"

class UListDataObject_Base;
class UCommonTextBlock;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class BUGDESTROYER_API UWidget_ListEntry_Base : public UCommonUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	//begin UUserWidget Interface
	virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, 
		const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;
	//end UUserWidget Interface
	
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "On List Entry Widget Hovered"))
	void BP_OnListEntryWidgetHovered(bool bWasHovered, bool bIsEntryWidgetStillSelected);
	void NativeOnListEntryWidgetHovered(bool bWasHovered);

protected:
	// The Blueprint child should override this function to handle the initialization needed
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Get Widget To Focus Fro Gamepad"))
	UWidget* BP_GetWidgetToFocusForGamepad() const;
	
	// The Blueprint child should override this function to handle the highlight when hovered or selected 
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Toggle Entry Widget Highlight State"))
	void BP_OnToggleEntryWidgetHighlightState(bool bShouldHighlight) const;
	
	// begin IUserObjectListEntry interface
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnEntryReleased() override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;
	//end IUserObjectListEntry interface
	
	//begin UUserWidget Interface
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	//end UUserWidget Interface
	
	// The child class should override this function to handle the initialization needed. Super call is expected.
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject);
	
	// The child class should override this function to update the UI values after the data object has been modified. Super call is expected.
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningListDataObject, EOptionsListDataModifyReason ModifyReason);
		
	// The child class should override this function to update the UI values after the data object has been modified. Super call is expected.
	virtual void OnOwningDependencyDataModified(UListDataObject_Base* OwningDependencyDataObject, EOptionsListDataModifyReason ModifyReason);
	
	// The child class should override this function to change editable state of the widget it owns. Super Call is Needed.
	virtual void OnToggleEditableState(bool bIsEditable);

	
	void SelectThisEntryWidget();
	
private:
	//**** bound widgets ****//
	UPROPERTY(BlueprintReadOnly, meta=(BindWidgetOptional, AllowPrivateAccess = "true"))
	UCommonTextBlock* CommonText_SettingDisplayName;
	//**** bound widgets ****//
	
	UPROPERTY(Transient)
	UListDataObject_Base* CachedOwningDataObject;
	
};
