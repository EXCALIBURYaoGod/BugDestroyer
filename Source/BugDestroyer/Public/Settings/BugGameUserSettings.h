// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "BugGameUserSettings.generated.h"

/**
 * 
 */
UCLASS()
class BUGDESTROYER_API UBugGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()
	
public:
	UBugGameUserSettings();
	
	static UBugGameUserSettings* Get();
	
	/** Gameplay Collection Tab **/
	UFUNCTION()
	FString GetCurrentGameDifficulty() const {return CurrentGameDifficulty;}
	
	UFUNCTION()
	void SetCurrentGameDifficulty(FString InCurrentGameDifficulty) { CurrentGameDifficulty = InCurrentGameDifficulty;}
	/** Gameplay Collection Tab **/
	
	/** Audio Collection Tab **/
	UFUNCTION()
	float GetOverallVolume() const { return OverallVolume; }
	UFUNCTION()
	void SetOverallVolume(float InVolume);
	UFUNCTION()
	float GetMusicVolume() const { return MusicVolume; }
	UFUNCTION()
	void SetMusicVolume(float InVolume);
	UFUNCTION()
	float GetSoundFXVolume() const { return SoundFXVolume; }
	UFUNCTION()
	void SetSoundFXVolume(float InVolume);
	UFUNCTION()
	bool GetAllowBackgroundAudio() const { return bAllowBackgroundAudio; }
	UFUNCTION()
	void SetAllowBackgroundAudio(bool InbAllowBackgroundAudio);
	UFUNCTION()
	bool GetUseHDRAudioMode() const { return bUseHDRAudioMode; }
	UFUNCTION()
	void SetUseHDRAudioMode(bool InbAllowBackgroundAudio);
	/** Audio Collection Tab **/

	/** Video Collection Tab **/
	UFUNCTION()
	float GetCurrentDisplayGamma() const;
	UFUNCTION()
	void SetCurrentDisplayGamma(float InDisplayGamma);
	/** Video Collection Tab **/
	
	/** Control Collection Tab **/
	UFUNCTION()
	float GetBaseLookSensitivity() const;
	UFUNCTION()
	void SetBaseLookSensitivity(float NewValue);
	UFUNCTION()
	float GetADSLookSensitivity() const;
	UFUNCTION()
	void SetADSLookSensitivity(float NewValue);
	/** Control Collection Tab **/
	
private:
	/** Gameplay Collection Tab **/
	UPROPERTY(Config)
	FString CurrentGameDifficulty;
	/** Gameplay Collection Tab **/
	
	/** Audio Collection Tab **/
	UPROPERTY(Config)
	float OverallVolume;
	UPROPERTY(Config)
	float MusicVolume;
	UPROPERTY(Config)
	float SoundFXVolume;
	UPROPERTY(Config)
	bool bAllowBackgroundAudio;
	UPROPERTY(Config)
	bool bUseHDRAudioMode;
	/** Audio Collection Tab **/
	
	/** Control Collection Tab **/
	UPROPERTY(Config)
	float BaseLookSensitivity;
	UPROPERTY(Config)
	float ADSLookSensitivity;
	/** Control Collection Tab **/
	
};
