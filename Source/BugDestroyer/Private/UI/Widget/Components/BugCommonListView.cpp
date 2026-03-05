// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Components/BugCommonListView.h"

#include "Editor/WidgetCompilerLog.h"
#include "Widget/Options/DataAsset_DataListEntryMapping.h"
#include "Widget/Options/DataObjects/ListDataObjcet_Collection.h"
#include "Widget/Options/ListEntries/Widget_ListEntry_Base.h"
#include "Widget/Options/DataObjects/ListDataObject_Base.h"

UUserWidget& UBugCommonListView::OnGenerateEntryWidgetInternal(UObject* Item,
                                                               TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (IsDesignTime())
	{
		return Super::OnGenerateEntryWidgetInternal(Item, DesiredEntryClass, OwnerTable);
	}
	
	if (TSubclassOf<UWidget_ListEntry_Base> FoundEntryWidgetClass = 
		DataListEntryMapping->FindEntryWidgetClassByDataObject(CastChecked<UListDataObject_Base>(Item)))
	{
		return GenerateTypedEntry<UWidget_ListEntry_Base>(FoundEntryWidgetClass, OwnerTable);
	}
	else
	{
		return Super::OnGenerateEntryWidgetInternal(Item, DesiredEntryClass, OwnerTable);	
	}
}

bool UBugCommonListView::OnIsSelectableOrNavigableInternal(UObject* FirstSelectedItem)
{
	return !FirstSelectedItem->IsA<UListDataObject_Collection>();
}


#if WITH_EDITOR
void UBugCommonListView::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);
	if (!DataListEntryMapping)
	{
		CompileLog.Error(FText::FromString(
			TEXT("The variable DataListEntryMapping has no valid data asset assigned") + 
			GetClass()->GetName() +
			TEXT(" needs a valid data asset fo function properly")
		));
	}
}
#endif
