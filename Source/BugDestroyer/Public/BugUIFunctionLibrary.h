// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BugUIFunctionLibrary.generated.h"

class UWidget_ActivatableBase;
/**
 * 
 */
UCLASS()
class BUGDESTROYER_API UBugUIFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "UI Function Library")
	static TSoftClassPtr<UWidget_ActivatableBase> GetSoftWidgetClassByTag(
		UPARAM(meta = (Categories = "Bug.Widget")) FGameplayTag InGameplayTag);
	
	UFUNCTION(BlueprintPure, Category = "UI Function Library")
	static TSoftObjectPtr<UTexture2D> GetSoftImageByTag(
		UPARAM(meta = (Categories = "Bug.Image")) FGameplayTag InGameplayTag);
	
};
