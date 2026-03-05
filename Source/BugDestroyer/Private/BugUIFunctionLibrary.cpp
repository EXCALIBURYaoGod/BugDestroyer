// Fill out your copyright notice in the Description page of Project Settings.


#include "BugUIFunctionLibrary.h"

#include "Settings/BugDestoryerDeveloperSettings.h"

TSoftClassPtr<UWidget_ActivatableBase> UBugUIFunctionLibrary::GetSoftWidgetClassByTag(FGameplayTag InGameplayTag)
{
	const UBugDestoryerDeveloperSettings* DeveloperSettings = GetDefault<UBugDestoryerDeveloperSettings>();
	checkf(DeveloperSettings->UIWidgetClassMap.Contains(InGameplayTag), 
		TEXT("could not find widget with widget tag &s"), *InGameplayTag.ToString());
	
	return DeveloperSettings->UIWidgetClassMap.FindRef(InGameplayTag);
}

TSoftObjectPtr<UTexture2D> UBugUIFunctionLibrary::GetSoftImageByTag(FGameplayTag InGameplayTag)
{
	const UBugDestoryerDeveloperSettings* DeveloperSettings = GetDefault<UBugDestoryerDeveloperSettings>();
	checkf(DeveloperSettings->OptionsScreenSoftImageMap.Contains(InGameplayTag),
		TEXT("could not find an image accociated with tag %s"), *InGameplayTag.ToString());
	
	return DeveloperSettings->OptionsScreenSoftImageMap.FindRef(InGameplayTag);
}
