// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonInputTypeEnum.h"
#include "ListDataObject_Base.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "ListDataObject_KeyRemap.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnKeyBindConflict, FName, ConflictingActionName, FKey, AttemptedKey);

class UEnhancedPlayerMappableKeyProfile;
class UEnhancedInputUserSettings;
/**
 * 
 */
UCLASS()
class BUGDESTROYER_API UListDataObject_KeyRemap : public UListDataObject_Base
{
	GENERATED_BODY()
	
public:
	void InitKeyRemapData(UEnhancedInputUserSettings* InOwningInputUserSettings, 
		UEnhancedPlayerMappableKeyProfile* InKeyProfile, 
		ECommonInputType InDesiredInputKeyType, 
		const FPlayerKeyMapping& InOwningPlayerKeyMapping);
	
	FSlateBrush GetIconFromCurrentKey() const;
	
	void BindNewInputKey(const FKey& InNewKey);

	// begin UListDataObject_Base interface
	virtual bool HasDefaultValue() const override;
	virtual bool CanResetBackToDefaultValue() const override;
	virtual bool TryResetBackToDefaultValue() override;
	//end UListDataObject_Base interface
	
	UPROPERTY(BlueprintAssignable, Category = "Key Remap")
	FOnKeyBindConflict OnKeyBindConflict;
	void ForceBindInputKey(FName ConflictingActionName, const FKey& InNewKey);
	
private:
	FPlayerKeyMapping* GetOwningKeyMapping() const;
	
	UPROPERTY(Transient)
	UEnhancedInputUserSettings* CachedOwningInputUserSettings;
	UPROPERTY(Transient)
	UEnhancedPlayerMappableKeyProfile* CachedOwningKeyProfile;
	ECommonInputType CachedDesiredInputType;
	FName CachedOwningMappingName;
	EPlayerMappableKeySlot CachedOwningMappableKeySlot;
	
public:
	FORCEINLINE ECommonInputType GetDesiredInputType() const { return CachedDesiredInputType; }
	
};
