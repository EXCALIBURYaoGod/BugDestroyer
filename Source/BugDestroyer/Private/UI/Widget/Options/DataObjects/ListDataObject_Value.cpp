// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Options/DataObjects/ListDataObject_Value.h"

void UListDataObject_Value::SetDataDynamicGetter(const TSharedPtr<FOptionsDataInteractionHelper> InDataDynamicGetter)
{
	DataDynamicGetter = InDataDynamicGetter;
}

void UListDataObject_Value::SetDataDynamicSetter(const TSharedPtr<FOptionsDataInteractionHelper> InDataDynamicSetter)
{
	DataDynamicSetter = InDataDynamicSetter;
}
