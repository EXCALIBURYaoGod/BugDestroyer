// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OptionsDataRegistry.generated.h"

class UListDataObject_Base;
class UListDataObject_Collection;
/**
 * 
 */
UCLASS()
class BUGDESTROYER_API UOptionsDataRegistry : public UObject
{
	GENERATED_BODY()
	
public:
	// Gets called by options screen right after the object of type UOptionsDataRegistry is created
	void InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer);
	
	const TArray<UListDataObject_Collection*>& GetRegisteredOptionsTabCollections() const{return RegisteredOptionsTabCollections;}
	
	TArray<UListDataObject_Base*> GetListSourceItemsBySelectedTabID(const FName& InSelectedTabID) const;
	
private:
	void FindChildListDataRecursively(UListDataObject_Base* InParentData, TArray<UListDataObject_Base*>& OutFoundChildListData) const;
	
	void InitGameplayCollectionTab();
	void InitAudioCollectionTab();
	void InitVideoCollectionTab();
	void InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer);
	
	// Video Category HelperFunctions
	FString GetOverallQualityDefaultValue() const;
	FString GetResolutionDefaultValue() const;
	FString GetGlobalIlluminationQualityDefaultValue() const;
	FString GetShadowQualityDefaultValue() const;
	FString GetAntiAliasingQualityDefaultValue() const;
	FString GetViewDistanceQualityDefaultValue() const;
	FString GetTextureQualityDefaultValue() const;
	FString GetVisualEffectQualityDefaultValue() const;
	FString GetReflectionQualityDefaultValue() const;
	FString GetPostProcessingQualityDefaultValue() const;
	FString GetVerticalSyncDefaultValue() const;
	FString GetFrameRateLimitDefaultValue() const;
	// Video Category HelperFunctions
	
	UPROPERTY(Transient)
	TArray<UListDataObject_Collection*> RegisteredOptionsTabCollections;
};
