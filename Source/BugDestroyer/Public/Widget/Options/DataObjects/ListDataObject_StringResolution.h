// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ListDataObject_String.h"
#include "ListDataObject_StringResolution.generated.h"

/**
 * 
 */
UCLASS()
class BUGDESTROYER_API UListDataObject_StringResolution : public UListDataObject_String
{
	GENERATED_BODY()
	
public:
	void InitResolutionValues();
	
protected:
	// begin UListDataObject_String Interface
	virtual void OnDataObjectInitialized() override;
	// end UListDataObject_String Interface
	
private:
	FString IntPointToValueString(const FIntPoint& InResolution) const;
	FText IntPointToDisplayText(const FIntPoint& InResolution) const;
	
	FString MaximumAllowedResolution;
	
public:
	FORCEINLINE FString GetMaximumAllowedResolution() const { return MaximumAllowedResolution; }
	
};
