// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Options/DataObjects/ListDataObjcet_Collection.h"

void UListDataObject_Collection::AddChildListData(UListDataObject_Base* InChildListData)
{
	// Notify the child list data to init itself
	InChildListData->InitDataObject();
	
	// set the child list data's parent to this
	InChildListData->SetParentData(this);
	
	ChildListDataArray.Add(InChildListData);
}

TArray<UListDataObject_Base*> UListDataObject_Collection::GetChildListDataObjects() const
{
	return ChildListDataArray;
}

bool UListDataObject_Collection::HasAnyChildListDataObjects() const
{
	return ChildListDataArray.IsEmpty() == false;
}
