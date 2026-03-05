// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Options/DataObjects/ListDataObject_StringResolution.h"

#include "DebugHelper.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Settings/BugGameUserSettings.h"

void UListDataObject_StringResolution::InitResolutionValues()
{
	TArray<FIntPoint> AvailableResolutions;
	
	UKismetSystemLibrary::GetSupportedFullscreenResolutions(AvailableResolutions);

	AvailableResolutions.Sort(
			[](const FIntPoint& A, const FIntPoint& B)->bool
			{
				return A.SizeSquared() < B.SizeSquared();
			}
		);
	
	for (const FIntPoint& Resolution : AvailableResolutions)
	{
		// Resolution Value from Dynamic Getter: (X = 1920, Y= 1080)
		AddDynamicOption(IntPointToValueString(Resolution), IntPointToDisplayText(Resolution));
	}
	
	MaximumAllowedResolution = IntPointToValueString(AvailableResolutions.Last());
	SetDefaultValueFromString(MaximumAllowedResolution);
	
}

void UListDataObject_StringResolution::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();
	
	if (!TrySetDisplayTextFromStringValue(CurrentStringValue))
	{
		CurrentDisplayText = IntPointToDisplayText(UBugGameUserSettings::Get()->GetScreenResolution());
	}
}

FString UListDataObject_StringResolution::IntPointToValueString(const FIntPoint& InResolution) const
{
	return FString::Printf(TEXT("X=%i, Y=%i"), InResolution.X, InResolution.Y);
}

FText UListDataObject_StringResolution::IntPointToDisplayText(const FIntPoint& InResolution) const
{
	const FString DisplayString = FString::Printf(TEXT("%i x %i"), InResolution.X, InResolution.Y);
	return FText::FromString(DisplayString);
}
