// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget_ActivatableBase.h"
#include "BugTypes/BugEnumTypes.h"
#include "Widget_OptionsScreen.generated.h"


class UListDataObject_Base;
class UBugCommonListView;
class UBugTabListWidgetBase;
class UOptionsDataRegistry;
class UWidget_OptionsDetailsView;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class BUGDESTROYER_API UWidget_OptionsScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()

	
protected:
	// begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	// end UUserWidget Interface
	
	// begin UCommonActivatableWidget Interface
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	// end UCommonActivatableWidget Interface
	
private:
	UOptionsDataRegistry* GetOrCreateDataRegistry();
	
	void OnResetBoundActionTriggerred();
	
	UFUNCTION()
	void OnOptionsTabSelected(FName TabID);
	
	void OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered);
	void OnListViewItemSelected(UObject* InSelectedItem);
	
	FString TryGetEntryWidgetClassName(UObject* InOwningListItem) const;
	
	void OnListViewListDataModified(UListDataObject_Base* ModifiedData, EOptionsListDataModifyReason ModifiedReason);
	
	/*** Bound Widgets ***/
	UPROPERTY(meta = (BindWidget))
	UBugTabListWidgetBase* TabListWidget_OptionsTabs;
	UPROPERTY(meta = (BindWidget))
	UBugCommonListView* CommonListView_OptionsListView;
	UPROPERTY(meta = (BindWidget))
	UWidget_OptionsDetailsView* DetailsView_ListEntryInfo;
	/*** Bound Widgets ***/
	
	UPROPERTY(Transient)
	UOptionsDataRegistry* CreatedOwningDataRegistry;
	
	UPROPERTY(EditDefaultsOnly, Category = "Bug Options Screen", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle ResetAction;
	FUIActionBindingHandle ResetActionHandle;
	
	UPROPERTY(Transient)
	TArray<UListDataObject_Base*> ResettableDataArray;
	
	bool bIsResettingData = false;
	
	
};
