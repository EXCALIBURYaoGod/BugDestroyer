// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Options/DataObjects/ListDataObject_KeyRemap.h"

#include "CommonInputBaseTypes.h"
#include "CommonInputSubsystem.h"
#include "DebugHelper.h"

void UListDataObject_KeyRemap::InitKeyRemapData(UEnhancedInputUserSettings* InOwningInputUserSettings,
                                                UEnhancedPlayerMappableKeyProfile* InKeyProfile, ECommonInputType InDesiredInputKeyType,
                                                const FPlayerKeyMapping& InOwningPlayerKeyMapping)
{
	CachedDesiredInputType = InDesiredInputKeyType;
	CachedOwningInputUserSettings = InOwningInputUserSettings;
	CachedOwningKeyProfile = InKeyProfile;
	CachedOwningMappingName = InOwningPlayerKeyMapping.GetMappingName();
	CachedOwningMappableKeySlot = InOwningPlayerKeyMapping.GetSlot();
}

FSlateBrush UListDataObject_KeyRemap::GetIconFromCurrentKey() const
{
	check(CachedOwningInputUserSettings);
	FSlateBrush FoundBrush;
	UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(CachedOwningInputUserSettings->GetLocalPlayer());
	check(CommonInputSubsystem);
	
	
	const bool bHasFoundBrush = UCommonInputPlatformSettings::Get()->TryGetInputBrush(
			FoundBrush,
			GetOwningKeyMapping()->GetCurrentKey(),
			CachedDesiredInputType,
			CommonInputSubsystem->GetCurrentGamepadName()
		);
	
	if (!bHasFoundBrush)
	{
		Debug::Print(TEXT("Unable to find an icon for the key") + GetOwningKeyMapping()->GetCurrentKey().GetDisplayName().ToString()
				+ TEXT(" Empty brush was applied. ")
			);
	}
	
	return FoundBrush;
}

void UListDataObject_KeyRemap::BindNewInputKey(const FKey& InNewKey)
{
	check(CachedOwningInputUserSettings);
	check(CachedOwningKeyProfile);
	
	// 检测按键冲突
	TArray<FName> ConflictingMappingNames;
	int32 FoundConflicts = CachedOwningKeyProfile->GetMappingNamesForKey(InNewKey, ConflictingMappingNames);
	FName RealConflictingActionName = NAME_None;
	if (FoundConflicts > 0)
	{
		for (const FName& ConflictingName : ConflictingMappingNames)
		{
			if (ConflictingName != CachedOwningMappingName)
			{
				RealConflictingActionName = ConflictingName;
				break;
			}
		}
	}
	if (RealConflictingActionName != NAME_None)
	{
		OnKeyBindConflict.Broadcast(RealConflictingActionName, InNewKey);
		return;
	}
	// 检测按键冲突
	
	// 绑定按键
	FMapPlayerKeyArgs KeyArgs;
	KeyArgs.MappingName = CachedOwningMappingName;
	KeyArgs.Slot = CachedOwningMappableKeySlot;
	KeyArgs.NewKey = InNewKey;
	
	FGameplayTagContainer Container;
	CachedOwningInputUserSettings->MapPlayerKey(KeyArgs, Container);
	CachedOwningInputUserSettings->SaveSettings();
	
	NotifyListDataModified(this);
	// 绑定按键
}

bool UListDataObject_KeyRemap::HasDefaultValue() const
{
	return GetOwningKeyMapping()->GetDefaultKey().IsValid();
}

bool UListDataObject_KeyRemap::CanResetBackToDefaultValue() const
{
	return HasDefaultValue() && GetOwningKeyMapping()->IsCustomized();
}

bool UListDataObject_KeyRemap::TryResetBackToDefaultValue()
{
	if (CanResetBackToDefaultValue())
	{
		check(CachedOwningInputUserSettings);
		GetOwningKeyMapping()->ResetToDefault();
		CachedOwningInputUserSettings->SaveSettings();
		NotifyListDataModified(this, EOptionsListDataModifyReason::ResetToDefault);
		return true;
	}
	return Super::TryResetBackToDefaultValue();
}

void UListDataObject_KeyRemap::ForceBindInputKey(FName ConflictingActionName, const FKey& InNewKey)
{
	check(CachedOwningInputUserSettings);

	FGameplayTagContainer FailureReason;
	
	FMapPlayerKeyArgs UnmapArgs;
	UnmapArgs.MappingName = ConflictingActionName;
	UnmapArgs.NewKey = InNewKey;
	UnmapArgs.Slot = EPlayerMappableKeySlot::First; 
	CachedOwningInputUserSettings->UnMapPlayerKey(UnmapArgs, FailureReason);
	
	FMapPlayerKeyArgs MapArgs;
	MapArgs.MappingName = CachedOwningMappingName;
	MapArgs.Slot = CachedOwningMappableKeySlot;
	MapArgs.NewKey = InNewKey;
	CachedOwningInputUserSettings->MapPlayerKey(MapArgs, FailureReason);
	
	CachedOwningInputUserSettings->SaveSettings();
	NotifyListDataModified(this);
}

FPlayerKeyMapping* UListDataObject_KeyRemap::GetOwningKeyMapping() const
{
	check(CachedOwningKeyProfile);
	FMapPlayerKeyArgs KeyArgs;
	KeyArgs.MappingName = CachedOwningMappingName;
	KeyArgs.Slot = CachedOwningMappableKeySlot;
	
	return CachedOwningKeyProfile->FindKeyMapping(KeyArgs);
	
}
