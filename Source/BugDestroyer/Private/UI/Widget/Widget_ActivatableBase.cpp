// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_ActivatableBase.h"

APlayerController* UWidget_ActivatableBase::GetOwningPlayerController()
{
	if (!CachedOwningPlayerController.IsValid())
	{
		CachedOwningPlayerController = GetOwningPlayer<APlayerController>();
	}
	return CachedOwningPlayerController.IsValid()? CachedOwningPlayerController.Get(): nullptr;
}
