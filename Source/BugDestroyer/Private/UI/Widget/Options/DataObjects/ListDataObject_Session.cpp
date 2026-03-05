// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Options/DataObjects/ListDataObject_Session.h"

void UListDataObject_Session::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();
	// 巧妙利用父类的宏生成函数：将房间名设为通用的 DisplayName
	SetDataDisplayName(FText::FromString(SessionInfo.ServerName));
	// 可以将内部索引转为 ID 以备不时之需
	SetDataID(FName(*FString::FromInt(SessionInfo.InternalIndex)));
	
}
