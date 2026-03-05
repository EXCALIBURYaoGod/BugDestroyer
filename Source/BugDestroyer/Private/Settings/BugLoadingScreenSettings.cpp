// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/BugLoadingScreenSettings.h"

#include "Blueprint/UserWidget.h"

TSubclassOf<UUserWidget> UBugLoadingScreenSettings::GetLoadingScreenWidgetClassChecked() const
{
	checkf(!SoftLoadingScreenWidgetClass.IsNull(), TEXT("UBugLoadingScreenWidgetClass "
													 "is not set in the project settings"));
	TSubclassOf<UUserWidget> LoadedLoadingScreenWidget = SoftLoadingScreenWidgetClass.LoadSynchronous();
	
	return LoadedLoadingScreenWidget;
	
}
