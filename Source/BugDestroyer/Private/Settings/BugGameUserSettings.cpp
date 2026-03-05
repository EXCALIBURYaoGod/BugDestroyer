// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/BugGameUserSettings.h"

UBugGameUserSettings::UBugGameUserSettings()
	: OverallVolume(1.f)
	, MusicVolume(1.f)
	, SoundFXVolume(1.f)
	, bAllowBackgroundAudio(false)
	, bUseHDRAudioMode(false)
{
	
}

UBugGameUserSettings* UBugGameUserSettings::Get()
{
	if (GEngine)
	{
		return CastChecked<UBugGameUserSettings>(GEngine->GetGameUserSettings());
	}
	return nullptr;
}

void UBugGameUserSettings::SetOverallVolume(float InVolume)
{
	OverallVolume = InVolume;
	
	// The Actual logic for controlling the settings goes here
}

void UBugGameUserSettings::SetMusicVolume(float InVolume)
{
	MusicVolume = InVolume;
	
	// The Actual logic for controlling the settings goes here
}

void UBugGameUserSettings::SetSoundFXVolume(float InVolume)
{
	SoundFXVolume = InVolume;
	
	// The Actual logic for controlling the settings goes here
}

void UBugGameUserSettings::SetAllowBackgroundAudio(bool InbAllowBackgroundAudio)
{
	bAllowBackgroundAudio = InbAllowBackgroundAudio;
	
	// The Actual logic for controlling the settings goes here
}

void UBugGameUserSettings::SetUseHDRAudioMode(bool InbAllowBackgroundAudio)
{
	bUseHDRAudioMode = InbAllowBackgroundAudio;
	
	// The Actual logic for controlling the settings goes here
}

float UBugGameUserSettings::GetCurrentDisplayGamma() const
{
	if (GEngine)
	{
		return GEngine->GetDisplayGamma();
	}
	return 0.0f;
}

void UBugGameUserSettings::SetCurrentDisplayGamma(float InDisplayGamma)
{
	if (GEngine)
	{
		GEngine->DisplayGamma = InDisplayGamma;
	}
}
