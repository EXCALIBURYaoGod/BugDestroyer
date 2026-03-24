// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Options/OptionsDataRegistry.h"

#include "BugGameplayTags.h"
#include "BugUIFunctionLibrary.h"
#include "DebugHelper.h"
#include "EnhancedInputSubsystems.h"
#include "BugTypes/BugStructTypes.h"
#include "Settings/BugGameUserSettings.h"
#include "Widget/Options/DataObjects/ListDataObjcet_Collection.h"
#include "Widget/Options/DataObjects/ListDataObject_Scalar.h"
#include "Widget/Options/DataObjects/ListDataObject_String.h"
#include "Widget/Options/DataObjects/ListDataObject_StringResolution.h"
#include "Internationalization/StringTableRegistry.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Widget/Options/DataObjects/ListDataObject_KeyRemap.h"

#define MAKE_OPTIONS_DATA_CONTROL(SetterOrGetterFuncName) \
	MakeShared<FOptionsDataInteractionHelper>(GET_FUNCTION_NAME_STRING_CHECKED(UBugGameUserSettings, SetterOrGetterFuncName))

#define GET_DESCRIPTION(InKey) LOCTABLE("/Game/UI/StringTables/ST_OptionsScreenDescription.ST_OptionsScreenDescription", InKey)


void UOptionsDataRegistry::InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer)
{
	InitGameplayCollectionTab();
	InitAudioCollectionTab();
	InitVideoCollectionTab();
	InitControlCollectionTab(InOwningLocalPlayer);
	
}

TArray<UListDataObject_Base*> UOptionsDataRegistry::GetListSourceItemsBySelectedTabID(
	const FName& InSelectedTabID) const
{
	UListDataObject_Collection* const* FoundTabCollectionPtr = RegisteredOptionsTabCollections.FindByPredicate(
		[InSelectedTabID](UListDataObject_Collection* AvailableTabCollection)->bool
		{
			return AvailableTabCollection->GetDataID() == InSelectedTabID;
		}
	);
	
	checkf(*FoundTabCollectionPtr, TEXT("No valid tab found under the ID %s"), *InSelectedTabID.ToString());
	
	UListDataObject_Collection* FoundTabCollection = *FoundTabCollectionPtr;
	
	TArray<UListDataObject_Base*> AllChildListItems;
	
	for (UListDataObject_Base* ChildListData : FoundTabCollection->GetChildListDataObjects())
	{
		if (!ChildListData)
		{
			continue;
		}
		AllChildListItems.Add(ChildListData);
		if (ChildListData->HasAnyChildListDataObjects())
		{
			FindChildListDataRecursively(ChildListData, AllChildListItems);
		}
	}
	
	return AllChildListItems;
}

void UOptionsDataRegistry::FindChildListDataRecursively(UListDataObject_Base* InParentData,
	TArray<UListDataObject_Base*>& OutFoundChildListData) const
{
	if (!InParentData || !InParentData->HasAnyChildListDataObjects())
	{
		return;
	}

	for (UListDataObject_Base* SubChildListData : InParentData->GetChildListDataObjects())
	{
		if (!SubChildListData)
		{
			continue;
		}
		OutFoundChildListData.Add(SubChildListData);

		if (SubChildListData->HasAnyChildListDataObjects())
		{
			FindChildListDataRecursively(SubChildListData, OutFoundChildListData);
		}
	}
	
}

void UOptionsDataRegistry::InitGameplayCollectionTab()
{
	UListDataObject_Collection* GameplayTabCollection = NewObject<UListDataObject_Collection>();
	GameplayTabCollection->SetDataID(FName("GameplayTabCollection"));
	GameplayTabCollection->SetDataDisplayName(FText::FromString(TEXT("Gameplay")));
	
	/* This is the full code for constructor data interactor helper
	 TSharedPtr<FOptionsDataInteractionHelper> ConstructedHelper = MakeShared<FOptionsDataInteractionHelper>(
		GET_FUNCTION_NAME_STRING_CHECKED(UBugGameUserSettings, GetCurrentGameDifficulty));
	*/
	
	
	// Game difficulty
	{
		UListDataObject_String* GameDifficulty = NewObject<UListDataObject_String>();
		GameDifficulty->SetDataID(FName("GameDifficulty"));
		GameDifficulty->SetDataDisplayName(FText::FromString(TEXT("Difficulty")));
		GameDifficulty->SetDescriptionRichText(FText::FromString(TEXT("Adjusts the difficulty of the game experience.\n\n"
																"<Bold>Easy:</> Focuses on the story experience. "
																			"Provides the most relaxing combat.\n\n"
																"<Bold>Medium:</> Offers slightly harder combat experience\n\n"
															 "<Bold>Hard:</> Offers a much more challenging combat experience\n\n"
															 "<Bold>Vert Hard:</> Provides the most challenging combat experience. "
															 "Not recommended for first play through.")));
		GameDifficulty->AddDynamicOption(TEXT("Easy"), FText::FromString(TEXT("Easy")));
		GameDifficulty->AddDynamicOption(TEXT("Medium"), FText::FromString(TEXT("Medium")));
		GameDifficulty->AddDynamicOption(TEXT("Hard"), FText::FromString(TEXT("Hard")));
		GameDifficulty->AddDynamicOption(TEXT("NeverEnd"), FText::FromString(TEXT("NeverEnd")));
		GameDifficulty->SetDefaultValueFromString(TEXT("Medium"));
		GameDifficulty->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetCurrentGameDifficulty));
		GameDifficulty->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetCurrentGameDifficulty));
		GameDifficulty->SetShouldApplyChangeImmediately(true);
		
		GameplayTabCollection->AddChildListData(GameDifficulty);
	}
	
	// Test Item
	{
		UListDataObject_String* TestItem = NewObject<UListDataObject_String>();
		TestItem->SetDataID(FName("TestItem"));
		TestItem->SetDataDisplayName(FText::FromString(TEXT("Test Item")));
		TestItem->SetSoftDescriptionImage(UBugUIFunctionLibrary::GetSoftImageByTag(BugGameplayTags::Bug_Image_TestImage));
		TestItem->SetDescriptionRichText(FText::FromString(TEXT("The image to display can be specified in the project settings."
														  "It can be anything the developer assigned in there.")));
		
		GameplayTabCollection->AddChildListData(TestItem);
	}
	
	RegisteredOptionsTabCollections.Add(GameplayTabCollection);
}

void UOptionsDataRegistry::InitAudioCollectionTab()
{
	UListDataObject_Collection* AudioTabCollection = NewObject<UListDataObject_Collection>();
	AudioTabCollection->SetDataID(FName("AudioTabCollection"));
	AudioTabCollection->SetDataDisplayName(FText::FromString(TEXT("Audio")));
	
	//Volume Category
	{
		UListDataObject_Collection* VolumeCategoryCollection = NewObject<UListDataObject_Collection>();
		VolumeCategoryCollection->SetDataID(FName("VolumeCategoryCollection"));
		VolumeCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Volume")));
		
		AudioTabCollection->AddChildListData(VolumeCategoryCollection);
		
		// Overall Volume
		{
			UListDataObject_Scalar* OverallVolume = NewObject<UListDataObject_Scalar>();
			OverallVolume->SetDataID(FName("OverallVolume"));
			OverallVolume->SetDataDisplayName(FText::FromString(TEXT("Overall Volume")));
			OverallVolume->SetDescriptionRichText(FText::FromString(TEXT("This is description for Overall Volume")));
			OverallVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			OverallVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));
			OverallVolume->SetSliderStepSize(0.01f);
			OverallVolume->SetDefaultValueFromString(LexToString(1.f));
			OverallVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
			OverallVolume->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimal()); // No Decimal: 50% , One Decimal: 50.5%
			OverallVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetOverallVolume));
			OverallVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetOverallVolume));
			OverallVolume->SetShouldApplyChangeImmediately(true);
			
			VolumeCategoryCollection->AddChildListData(OverallVolume);
		}
		
		// Music Volume
		{
			UListDataObject_Scalar* MusicVolume = NewObject<UListDataObject_Scalar>();
			MusicVolume->SetDataID(FName("MusicVolume"));
			MusicVolume->SetDataDisplayName(FText::FromString(TEXT("Music Volume")));
			MusicVolume->SetDescriptionRichText(FText::FromString(TEXT("This is description for Music Volume")));
			MusicVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			MusicVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));
			MusicVolume->SetSliderStepSize(0.01f);
			MusicVolume->SetDefaultValueFromString(LexToString(1.f));
			MusicVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
			MusicVolume->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimal()); // No Decimal: 50% , One Decimal: 50.5%
			MusicVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetMusicVolume));
			MusicVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetMusicVolume));
			MusicVolume->SetShouldApplyChangeImmediately(true);
			
			VolumeCategoryCollection->AddChildListData(MusicVolume);
		}
		
		// Sound FX Volume
		{
			UListDataObject_Scalar* SoundFXVolume = NewObject<UListDataObject_Scalar>();
			SoundFXVolume->SetDataID(FName("SoundFXVolume"));
			SoundFXVolume->SetDataDisplayName(FText::FromString(TEXT("Sound FX Volume")));
			SoundFXVolume->SetDescriptionRichText(FText::FromString(TEXT("This is description for Sound FX Volume")));
			SoundFXVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			SoundFXVolume->SetOutputValueRange(TRange<float>(0.f, 2.f));
			SoundFXVolume->SetSliderStepSize(0.01f);
			SoundFXVolume->SetDefaultValueFromString(LexToString(1.f));
			SoundFXVolume->SetDisplayNumericType(ECommonNumericType::Percentage);
			SoundFXVolume->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimal()); // No Decimal: 50% , One Decimal: 50.5%
			SoundFXVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetSoundFXVolume));
			SoundFXVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetSoundFXVolume));
			SoundFXVolume->SetShouldApplyChangeImmediately(true);
			
			VolumeCategoryCollection->AddChildListData(SoundFXVolume);
		}
		
	}
	
	//Sound Category
	{
		UListDataObject_Collection* SoundCategoryCollection = NewObject<UListDataObject_Collection>();
		SoundCategoryCollection->SetDataID(FName("SoundCategoryCollection"));
		SoundCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Sound")));
		
		//Allow Background Audio
		{	
			UListDataObject_StringBool* AllowBackgroundAudio = NewObject<UListDataObject_StringBool>();
			AllowBackgroundAudio->SetDataID(FName("AllowBackgroundAudio"));
			AllowBackgroundAudio->SetDataDisplayName(FText::FromString(TEXT("Allow Background Audio")));
			AllowBackgroundAudio->OverrideTrueDisplayText(FText::FromString(TEXT("Enabled")));
			AllowBackgroundAudio->OverrideFalseDisplayText(FText::FromString(TEXT("Disabled")));
			AllowBackgroundAudio->SetFalseAsDefaultValue();
			AllowBackgroundAudio->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetAllowBackgroundAudio));
			AllowBackgroundAudio->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetAllowBackgroundAudio));
			AllowBackgroundAudio->SetShouldApplyChangeImmediately(true);
			
			SoundCategoryCollection->AddChildListData(AllowBackgroundAudio);
		}
		
		//Use HDR Audio
		{	
			UListDataObject_StringBool* UseHDRAudioMode = NewObject<UListDataObject_StringBool>();
			UseHDRAudioMode->SetDataID(FName("UseHDRAudioMode"));
			UseHDRAudioMode->SetDataDisplayName(FText::FromString(TEXT("Use HDRAudio Mode")));
			UseHDRAudioMode->OverrideTrueDisplayText(FText::FromString(TEXT("Enabled")));
			UseHDRAudioMode->OverrideFalseDisplayText(FText::FromString(TEXT("Disabled")));
			UseHDRAudioMode->SetFalseAsDefaultValue();
			UseHDRAudioMode->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetUseHDRAudioMode));
			UseHDRAudioMode->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetUseHDRAudioMode));
			UseHDRAudioMode->SetShouldApplyChangeImmediately(true);
			
			SoundCategoryCollection->AddChildListData(UseHDRAudioMode);
		}
		
		
		AudioTabCollection->AddChildListData(SoundCategoryCollection);
	}
	
	RegisteredOptionsTabCollections.Add(AudioTabCollection);
}

void UOptionsDataRegistry::InitVideoCollectionTab()
{
	UListDataObject_Collection* VideoTabCollection = NewObject<UListDataObject_Collection>();
	VideoTabCollection->SetDataID(FName("VideoTabCollection"));
	VideoTabCollection->SetDataDisplayName(FText::FromString(TEXT("Video")));
	
	UListDataObject_StringEnum* CreatedWindowMode = nullptr;
	
	FOptionsDataEditConditionDescriptor PackagedBuildOnlyCondition;
	PackagedBuildOnlyCondition.SetEditConditionFunc(
		[]()->bool
			{
				const bool bIsInEditor = GIsEditor || GIsPlayInEditorWorld;
				return !bIsInEditor;
			}
		);
	
	PackagedBuildOnlyCondition.SetDisabledRichReason(
		TEXT("\n\n<Disabled>This setting can only be adjusted in a package build.</>"));
	
	// Display Category Collection
	{
		UListDataObject_Collection* DisplayCategoryCollection = NewObject<UListDataObject_Collection>();
		DisplayCategoryCollection->SetDataID(FName("DisplayCategoryCollection"));
		DisplayCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Display")));
		
		//Window Mode
		{
			UListDataObject_StringEnum* WindowMode = NewObject<UListDataObject_StringEnum>();
			WindowMode->SetDataID(FName("WindowMode"));
			WindowMode->SetDataDisplayName(FText::FromString(TEXT("Window Mode")));
			WindowMode->SetDescriptionRichText(GET_DESCRIPTION("WindowModeDescKey"));
			WindowMode->AddEnumOption(EWindowMode::Fullscreen, FText::FromString(TEXT("FullScreen Mode")));
			WindowMode->AddEnumOption(EWindowMode::WindowedFullscreen, FText::FromString(TEXT("Borderless Windowed")));
			WindowMode->AddEnumOption(EWindowMode::Windowed, FText::FromString(TEXT("Windowed")));
			WindowMode->SetDefaultValueFromEnumOption(EWindowMode::WindowedFullscreen);
			WindowMode->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetFullscreenMode));
			WindowMode->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetFullscreenMode));
			WindowMode->SetShouldApplyChangeImmediately(true);
			
			WindowMode->AddEditCondition(PackagedBuildOnlyCondition);
			
			CreatedWindowMode = WindowMode;
			
			DisplayCategoryCollection->AddChildListData(WindowMode);
		}
		
		//Screen Resolution
		{
			UListDataObject_StringResolution* ScreenResolution = NewObject<UListDataObject_StringResolution>();
			ScreenResolution->SetDataID(FName("ScreenResolution"));
			ScreenResolution->SetDataDisplayName(FText::FromString(TEXT("Screen Resolution")));
			ScreenResolution->SetDescriptionRichText(GET_DESCRIPTION("ScreenResolutionsDescKey"));
			ScreenResolution->InitResolutionValues();
			ScreenResolution->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetScreenResolution));
			ScreenResolution->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetScreenResolution));
			ScreenResolution->SetShouldApplyChangeImmediately(true);
			ScreenResolution->AddEditCondition(PackagedBuildOnlyCondition);
			FOptionsDataEditConditionDescriptor WindowModeEditCondition;
			WindowModeEditCondition.SetEditConditionFunc(
					[CreatedWindowMode]()->bool
					{
						const bool bIsBoderLessWindow = CreatedWindowMode->GetCurrentValueAsEnum<EWindowMode::Type>() 
						== EWindowMode::WindowedFullscreen;
						return !bIsBoderLessWindow;
					}
				);
			WindowModeEditCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>Screen "
				"Resolution is not adjustable when the 'window mode' "
				"is set to Borderless Window.</>"));
			WindowModeEditCondition.SetDisabledForcedStringValue(ScreenResolution->GetMaximumAllowedResolution());
			ScreenResolution->AddEditCondition(WindowModeEditCondition);
			ScreenResolution->AddEditDependencyData(CreatedWindowMode);
			
			DisplayCategoryCollection->AddChildListData(ScreenResolution);
		}
		
		VideoTabCollection->AddChildListData(DisplayCategoryCollection);
	}
	
	// Graphics Category
	{
		UListDataObject_Collection* GraphicsCategoryCollection = NewObject<UListDataObject_Collection>();
		GraphicsCategoryCollection->SetDataID(FName("GraphicsCategoryCollection"));
		GraphicsCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Graphics")));
		
		//Display Gamma
		{
			UListDataObject_Scalar* DisplayGamma = NewObject<UListDataObject_Scalar>();
			DisplayGamma->SetDataID(FName("DisplayGamma"));
			DisplayGamma->SetDataDisplayName(FText::FromString(TEXT("Brightness")));
			DisplayGamma->SetDescriptionRichText(GET_DESCRIPTION("DisplayGammaDescKey"));
			DisplayGamma->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			DisplayGamma->SetOutputValueRange(TRange<float>(1.7f, 2.7f)); // the default value Unreal has is : 2.2f
			DisplayGamma->SetDisplayNumericType(ECommonNumericType::Percentage);
			DisplayGamma->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());
			DisplayGamma->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetCurrentDisplayGamma));
			DisplayGamma->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetCurrentDisplayGamma));
			DisplayGamma->SetDefaultValueFromString(LexToString(2.2f));
			DisplayGamma->SetShouldApplyChangeImmediately(true);
			
			GraphicsCategoryCollection->AddChildListData(DisplayGamma);
		}
		
		UListDataObject_StringInteger* CachedOverallQuality = nullptr;
		
		// Overall Quality
		{
			UListDataObject_StringInteger* OverallQuality = NewObject<UListDataObject_StringInteger>();
			OverallQuality->SetDataID(FName("OverallQuality"));
			OverallQuality->SetDataDisplayName(FText::FromString(TEXT("Overall Quality")));
			OverallQuality->SetDescriptionRichText(GET_DESCRIPTION("OverallQualityDescKey"));
			OverallQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			OverallQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			OverallQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
			OverallQuality->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			OverallQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			OverallQuality->SetDefaultValueFromString(GetOverallQualityDefaultValue());
			OverallQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetOverallScalabilityLevel));
			OverallQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetOverallScalabilityLevel));
			OverallQuality->SetShouldApplyChangeImmediately(true);
			
			
			GraphicsCategoryCollection->AddChildListData(OverallQuality);
			CachedOverallQuality = OverallQuality;
		}
		
		// Resolution Scale
		{
			UListDataObject_Scalar* ResolutionScale = NewObject<UListDataObject_Scalar>();
			ResolutionScale->SetDataID(FName("ResolutionScale"));
			ResolutionScale->SetDataDisplayName(FText::FromString(TEXT("3D Resolution")));
			ResolutionScale->SetDescriptionRichText(GET_DESCRIPTION("ResolutionScaleDescKey"));
			ResolutionScale->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			ResolutionScale->SetOutputValueRange(TRange<float>(0.f, 1.f));
			ResolutionScale->SetDisplayNumericType(ECommonNumericType::Percentage);
			ResolutionScale->SetNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());
			ResolutionScale->SetDefaultValueFromString(GetResolutionDefaultValue());
			ResolutionScale->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetResolutionScaleNormalized));
			ResolutionScale->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetResolutionScaleNormalized));
			ResolutionScale->SetShouldApplyChangeImmediately(true);
			
			ResolutionScale->AddEditDependencyData(CachedOverallQuality);
			
			GraphicsCategoryCollection->AddChildListData(ResolutionScale);
			
		}
		
		//Global Illumination Quality
		{
			UListDataObject_StringInteger* GlobalIlluminationQuality = NewObject<UListDataObject_StringInteger>();
			GlobalIlluminationQuality->SetDataID(FName("GlobalIlluminationQuality"));
			GlobalIlluminationQuality->SetDataDisplayName(FText::FromString(TEXT("Global Illumination")));
			GlobalIlluminationQuality->SetDescriptionRichText(GET_DESCRIPTION("GlobalIlluminationQualityDescKey"));
			GlobalIlluminationQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			GlobalIlluminationQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			GlobalIlluminationQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
			GlobalIlluminationQuality->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			GlobalIlluminationQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			GlobalIlluminationQuality->SetDefaultValueFromString(GetGlobalIlluminationQualityDefaultValue());
			GlobalIlluminationQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetGlobalIlluminationQuality));
			GlobalIlluminationQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetGlobalIlluminationQuality));
			GlobalIlluminationQuality->SetShouldApplyChangeImmediately(true);
			
			GlobalIlluminationQuality->AddEditDependencyData(CachedOverallQuality);
			CachedOverallQuality->AddEditDependencyData(GlobalIlluminationQuality);
			
			GraphicsCategoryCollection->AddChildListData(GlobalIlluminationQuality);
		}
		
		//Shadow Quality
		{
			UListDataObject_StringInteger* ShadowQuality = NewObject<UListDataObject_StringInteger>();
			ShadowQuality->SetDataID(FName("ShadowQuality"));
			ShadowQuality->SetDataDisplayName(FText::FromString(TEXT("Shadow")));
			ShadowQuality->SetDescriptionRichText(GET_DESCRIPTION("ShadowQualityDescKey"));
			ShadowQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			ShadowQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			ShadowQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
			ShadowQuality->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			ShadowQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			ShadowQuality->SetDefaultValueFromString(GetShadowQualityDefaultValue());
			ShadowQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetShadowQuality));
			ShadowQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetShadowQuality));
			ShadowQuality->SetShouldApplyChangeImmediately(true);
			
			ShadowQuality->AddEditDependencyData(CachedOverallQuality);
			CachedOverallQuality->AddEditDependencyData(ShadowQuality);
			
			GraphicsCategoryCollection->AddChildListData(ShadowQuality);
		}
		
		//AntiAliasing Quality
		{
			UListDataObject_StringInteger* AntiAliasingQuality = NewObject<UListDataObject_StringInteger>();
			AntiAliasingQuality->SetDataID(FName("AntiAliasingQuality"));
			AntiAliasingQuality->SetDataDisplayName(FText::FromString(TEXT("AntiAliasing")));
			AntiAliasingQuality->SetDescriptionRichText(GET_DESCRIPTION("AntiAliasingDescKey"));
			AntiAliasingQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			AntiAliasingQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			AntiAliasingQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
			AntiAliasingQuality->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			AntiAliasingQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			AntiAliasingQuality->SetDefaultValueFromString(GetAntiAliasingQualityDefaultValue());
			AntiAliasingQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetAntiAliasingQuality));
			AntiAliasingQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetAntiAliasingQuality));
			AntiAliasingQuality->SetShouldApplyChangeImmediately(true);
			
			AntiAliasingQuality->AddEditDependencyData(CachedOverallQuality);
			CachedOverallQuality->AddEditDependencyData(AntiAliasingQuality);
			
			GraphicsCategoryCollection->AddChildListData(AntiAliasingQuality);
		}
		
		//View Distance Quality
		{
			UListDataObject_StringInteger* ViewDistanceQuality = NewObject<UListDataObject_StringInteger>();
			ViewDistanceQuality->SetDataID(FName("ViewDistanceQuality"));
			ViewDistanceQuality->SetDataDisplayName(FText::FromString(TEXT("View Distance")));
			ViewDistanceQuality->SetDescriptionRichText(GET_DESCRIPTION("ViewDistanceDescKey"));
			ViewDistanceQuality->AddIntegerOption(0, FText::FromString(TEXT("Near")));
			ViewDistanceQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			ViewDistanceQuality->AddIntegerOption(2, FText::FromString(TEXT("Far")));
			ViewDistanceQuality->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			ViewDistanceQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			ViewDistanceQuality->SetDefaultValueFromString(GetViewDistanceQualityDefaultValue());
			ViewDistanceQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetViewDistanceQuality));
			ViewDistanceQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetViewDistanceQuality));
			ViewDistanceQuality->SetShouldApplyChangeImmediately(true);
			
			ViewDistanceQuality->AddEditDependencyData(CachedOverallQuality);
			CachedOverallQuality->AddEditDependencyData(ViewDistanceQuality);
			
			GraphicsCategoryCollection->AddChildListData(ViewDistanceQuality);
		}
		
		// Texture Quality
		{
			UListDataObject_StringInteger* TextureQuality = NewObject<UListDataObject_StringInteger>();
			TextureQuality->SetDataID(FName("TextureQuality"));
			TextureQuality->SetDataDisplayName(FText::FromString(TEXT("Texture")));
			TextureQuality->SetDescriptionRichText(GET_DESCRIPTION("TextureQualityDescKey"));
			TextureQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			TextureQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			TextureQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
			TextureQuality->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			TextureQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			TextureQuality->SetDefaultValueFromString(GetTextureQualityDefaultValue());
			TextureQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetTextureQuality));
			TextureQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetTextureQuality));
			TextureQuality->SetShouldApplyChangeImmediately(true);
			
			TextureQuality->AddEditDependencyData(CachedOverallQuality);
			CachedOverallQuality->AddEditDependencyData(TextureQuality);
			
			GraphicsCategoryCollection->AddChildListData(TextureQuality);
		}
		
		// Visual Effects Quality
		{
			UListDataObject_StringInteger* VisualEffectQuality = NewObject<UListDataObject_StringInteger>();
			VisualEffectQuality->SetDataID(FName("VisualEffectQuality"));
			VisualEffectQuality->SetDataDisplayName(FText::FromString(TEXT("Visual Effect")));
			VisualEffectQuality->SetDescriptionRichText(GET_DESCRIPTION("VisualEffectQualityDescKey"));
			VisualEffectQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			VisualEffectQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			VisualEffectQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
			VisualEffectQuality->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			VisualEffectQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			VisualEffectQuality->SetDefaultValueFromString(GetVisualEffectQualityDefaultValue());
			VisualEffectQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetVisualEffectQuality));
			VisualEffectQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetVisualEffectQuality));
			VisualEffectQuality->SetShouldApplyChangeImmediately(true);
			
			VisualEffectQuality->AddEditDependencyData(CachedOverallQuality);
			CachedOverallQuality->AddEditDependencyData(VisualEffectQuality);
			
			GraphicsCategoryCollection->AddChildListData(VisualEffectQuality);
		}
		
		// Reflection Quality
		{
			UListDataObject_StringInteger* ReflectionQuality = NewObject<UListDataObject_StringInteger>();
			ReflectionQuality->SetDataID(FName("ReflectionQuality"));
			ReflectionQuality->SetDataDisplayName(FText::FromString(TEXT("Reflection")));
			ReflectionQuality->SetDescriptionRichText(GET_DESCRIPTION("ReflectionQualityDescKey"));
			ReflectionQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			ReflectionQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			ReflectionQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
			ReflectionQuality->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			ReflectionQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			ReflectionQuality->SetDefaultValueFromString(GetReflectionQualityDefaultValue());
			ReflectionQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetReflectionQuality));
			ReflectionQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetReflectionQuality));
			ReflectionQuality->SetShouldApplyChangeImmediately(true);
			
			ReflectionQuality->AddEditDependencyData(CachedOverallQuality);
			CachedOverallQuality->AddEditDependencyData(ReflectionQuality);
			
			GraphicsCategoryCollection->AddChildListData(ReflectionQuality);
		}
		
		// Post Processing Quality
		{
			UListDataObject_StringInteger* PostProcessingQuality = NewObject<UListDataObject_StringInteger>();
			PostProcessingQuality->SetDataID(FName("PostProcessingQuality"));
			PostProcessingQuality->SetDataDisplayName(FText::FromString(TEXT("Post Processing")));
			PostProcessingQuality->SetDescriptionRichText(GET_DESCRIPTION("PostProcessingQualityDescKey"));
			PostProcessingQuality->AddIntegerOption(0, FText::FromString(TEXT("Low")));
			PostProcessingQuality->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
			PostProcessingQuality->AddIntegerOption(2, FText::FromString(TEXT("High")));
			PostProcessingQuality->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
			PostProcessingQuality->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
			PostProcessingQuality->SetDefaultValueFromString(GetPostProcessingQualityDefaultValue());
			PostProcessingQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetPostProcessingQuality));
			PostProcessingQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetPostProcessingQuality));
			PostProcessingQuality->SetShouldApplyChangeImmediately(true);
			
			PostProcessingQuality->AddEditDependencyData(CachedOverallQuality);
			CachedOverallQuality->AddEditDependencyData(PostProcessingQuality);
			
			GraphicsCategoryCollection->AddChildListData(PostProcessingQuality);
		}
		
		VideoTabCollection->AddChildListData(GraphicsCategoryCollection);
	}
	
	// Advanced Graphics Category
	{
		UListDataObject_Collection* AdvancedGraphicsCategoryCollection = NewObject<UListDataObject_Collection>();
		AdvancedGraphicsCategoryCollection->SetDataID(FName("AdvancedGraphicsCategoryCollection"));
		AdvancedGraphicsCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Advanced Graphics")));
		
		//Vertical Sync
		{
			UListDataObject_StringBool* VerticalSync = NewObject<UListDataObject_StringBool>();
			VerticalSync->SetDataID(FName("VerticalSync"));
			VerticalSync->SetDataDisplayName(FText::FromString(TEXT("V-Sync")));
			VerticalSync->SetDescriptionRichText(GET_DESCRIPTION("VerticalSyncDescKey"));
			VerticalSync->SetDefaultValueFromString(GetVerticalSyncDefaultValue());
			VerticalSync->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(IsVSyncEnabled));
			VerticalSync->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetVSyncEnabled));
			VerticalSync->SetShouldApplyChangeImmediately(true);
			
			FOptionsDataEditConditionDescriptor FullScreenOnlyCondition;
			FullScreenOnlyCondition.SetEditConditionFunc(
					[CreatedWindowMode]()->bool
					{
						return CreatedWindowMode->GetCurrentValueAsEnum<EWindowMode::Type>() == EWindowMode::Fullscreen;
					}
				);
			FullScreenOnlyCondition.SetDisabledRichReason(TEXT("\n\n<Disabled>This feature only works if the 'Window Mode' is set to 'Fullscreen' .</>"));
			FullScreenOnlyCondition.SetDisabledForcedStringValue(TEXT("false"));
			VerticalSync->AddEditCondition(FullScreenOnlyCondition);
			
			AdvancedGraphicsCategoryCollection->AddChildListData(VerticalSync);
		}
		
		//Frame Rate Limit
		{
			UListDataObject_String* FrameRateLimit = NewObject<UListDataObject_String>();
			FrameRateLimit->SetDataID(FName("FrameRateLimit"));
			FrameRateLimit->SetDataDisplayName(FText::FromString(TEXT("Frame Rate")));
			FrameRateLimit->SetDescriptionRichText(GET_DESCRIPTION("FrameRateLimitDescKey"));
			FrameRateLimit->AddDynamicOption(LexToString(30.f), FText::FromString(TEXT("30 FPS")));
			FrameRateLimit->AddDynamicOption(LexToString(60.f), FText::FromString(TEXT("60 FPS")));
			FrameRateLimit->AddDynamicOption(LexToString(90.f), FText::FromString(TEXT("90 FPS")));
			FrameRateLimit->AddDynamicOption(LexToString(120.f), FText::FromString(TEXT("120 FPS")));
			FrameRateLimit->AddDynamicOption(LexToString(0.f), FText::FromString(TEXT("No Limit")));
			FrameRateLimit->SetDefaultValueFromString(GetFrameRateLimitDefaultValue());
			FrameRateLimit->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetFrameRateLimit));
			FrameRateLimit->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetFrameRateLimit));
			FrameRateLimit->SetShouldApplyChangeImmediately(true);
			
			AdvancedGraphicsCategoryCollection->AddChildListData(FrameRateLimit);
			
		}
		
		VideoTabCollection->AddChildListData(AdvancedGraphicsCategoryCollection);
	}
	
	RegisteredOptionsTabCollections.Add(VideoTabCollection);
}

void UOptionsDataRegistry::InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer)
{
	UListDataObject_Collection* ControlTabCollection = NewObject<UListDataObject_Collection>();
	ControlTabCollection->SetDataID(FName("ControlTabCollection"));
	ControlTabCollection->SetDataDisplayName(FText::FromString(TEXT("Control")));
	
	UEnhancedInputLocalPlayerSubsystem* EISubsystem = 
		InOwningLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(EISubsystem);
	UEnhancedInputUserSettings* EIUserSettings = EISubsystem->GetUserSettings();
	
	//	Keyboard Mouse Category
	{
		UListDataObject_Collection* KeyboardMouseCategoryCollection = NewObject<UListDataObject_Collection>();
		KeyboardMouseCategoryCollection->SetDataID(FName("KeyboardMouseCategoryCollection"));
		KeyboardMouseCategoryCollection->SetDataDisplayName(FText::FromString(TEXT("Keyboard & Mouse")));
		
		
		
		// Keyboard Mouse Inputs
		{
			// Mouse Sensitivity
			{
			    // 基础腰射灵敏度 (Base Look Sensitivity)
			    UListDataObject_Scalar* BaseSensitivity = NewObject<UListDataObject_Scalar>();
			    BaseSensitivity->SetDataID(FName("BaseLookSensitivity"));
			    BaseSensitivity->SetDataDisplayName(FText::FromString(TEXT("Base Look Sensitivity")));
			    BaseSensitivity->SetDescriptionRichText(FText::FromString(TEXT("Adjust the mouse sensitivity when not aiming.")));
			    BaseSensitivity->SetDisplayValueRange(TRange<float>(0.1f, 10.f));
			    BaseSensitivity->SetOutputValueRange(TRange<float>(0.1f, 10.f));
			    BaseSensitivity->SetSliderStepSize(0.1f);
			    BaseSensitivity->SetDefaultValueFromString(LexToString(1.f));
			    BaseSensitivity->SetDisplayNumericType(ECommonNumericType::Number); 
			    BaseSensitivity->SetNumberFormattingOptions(UListDataObject_Scalar::WithDecimal(1)); 
			    BaseSensitivity->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetBaseLookSensitivity));
			    BaseSensitivity->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetBaseLookSensitivity));
			    BaseSensitivity->SetShouldApplyChangeImmediately(true);

			    KeyboardMouseCategoryCollection->AddChildListData(BaseSensitivity);

			    // 开镜灵敏度 (ADS Look Sensitivity)
			    UListDataObject_Scalar* ADSSensitivity = NewObject<UListDataObject_Scalar>();
			    ADSSensitivity->SetDataID(FName("ADSLookSensitivity"));
			    ADSSensitivity->SetDataDisplayName(FText::FromString(TEXT("ADS Look Multiplier")));
			    ADSSensitivity->SetDescriptionRichText(FText::FromString(TEXT("Adjust the mouse sensitivity multiplier when aiming down sights.")));
			    ADSSensitivity->SetDisplayValueRange(TRange<float>(0.1f, 2.f));
			    ADSSensitivity->SetOutputValueRange(TRange<float>(0.1f, 2.f));
			    ADSSensitivity->SetSliderStepSize(0.1f);
			    ADSSensitivity->SetDefaultValueFromString(LexToString(0.5f));
			    ADSSensitivity->SetDisplayNumericType(ECommonNumericType::Number);
			    ADSSensitivity->SetNumberFormattingOptions(UListDataObject_Scalar::WithDecimal(1)); 
			    // 绑定后台数据的 Getter 和 Setter
			    ADSSensitivity->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetADSLookSensitivity));
			    ADSSensitivity->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetADSLookSensitivity));
			    ADSSensitivity->SetShouldApplyChangeImmediately(true);

			    KeyboardMouseCategoryCollection->AddChildListData(ADSSensitivity);
			}
			
			// Keyboard Inputs
			{
				FPlayerMappableKeyQueryOptions KeyboardMouseOnly;
				KeyboardMouseOnly.KeyToMatch = EKeys::S;
				KeyboardMouseOnly.bMatchBasicKeyTypes = true;
			
				/*FPlayerMappableKeyQueryOptions GamepadOnly;
				GamepadOnly.KeyToMatch = EKeys::Gamepad_FaceButton_Bottom;
				GamepadOnly.bMatchBasicKeyTypes = true;*/
			
				for (const auto& ProfilePair : EIUserSettings->GetAllAvailableKeyProfiles())
				{
					UEnhancedPlayerMappableKeyProfile* MappableKeyProfile = ProfilePair.Value;
					check(MappableKeyProfile);
					for (const auto& MappingRowPair : MappableKeyProfile->GetPlayerMappingRows())
					{
						for (const auto& KeyMapping : MappingRowPair.Value.Mappings)
						{
							if (MappableKeyProfile->DoesMappingPassQueryOptions(KeyMapping, KeyboardMouseOnly))
							{
								UListDataObject_KeyRemap* KeyRemapDataObject = NewObject<UListDataObject_KeyRemap>();
								KeyRemapDataObject->SetDataID(KeyMapping.GetMappingName());
								KeyRemapDataObject->SetDataDisplayName(KeyMapping.GetDisplayName());
								KeyRemapDataObject->InitKeyRemapData(
										EIUserSettings,
										MappableKeyProfile,
										ECommonInputType::MouseAndKeyboard,
										KeyMapping
									);
							
								KeyboardMouseCategoryCollection->AddChildListData(KeyRemapDataObject);
							}
						}
					}
				}
			}
		}
		
		ControlTabCollection->AddChildListData(KeyboardMouseCategoryCollection);
		
		
	}
	
	//	Gamepad Category
	{
		UListDataObject_Collection* GamepadCollection = NewObject<UListDataObject_Collection>();
		GamepadCollection->SetDataID(FName("GamepadCollection"));
		GamepadCollection->SetDataDisplayName(FText::FromString(TEXT("Gamepad")));
		
		// Gamepad Inputs
		{
			FPlayerMappableKeyQueryOptions GamepadOnly;
			GamepadOnly.KeyToMatch = EKeys::Gamepad_FaceButton_Bottom;
			GamepadOnly.bMatchBasicKeyTypes = true;
			
			for (const auto& ProfilePair : EIUserSettings->GetAllAvailableKeyProfiles())
			{
				UEnhancedPlayerMappableKeyProfile* MappableKeyProfile = ProfilePair.Value;
				check(MappableKeyProfile);
				for (const auto& MappingRowPair : MappableKeyProfile->GetPlayerMappingRows())
				{
					for (const auto& KeyMapping : MappingRowPair.Value.Mappings)
					{
						if (MappableKeyProfile->DoesMappingPassQueryOptions(KeyMapping, GamepadOnly))
						{
							UListDataObject_KeyRemap* KeyRemapDataObject = NewObject<UListDataObject_KeyRemap>();
							KeyRemapDataObject->SetDataID(KeyMapping.GetMappingName());
							KeyRemapDataObject->SetDataDisplayName(KeyMapping.GetDisplayName());
							KeyRemapDataObject->InitKeyRemapData(
									EIUserSettings,
									MappableKeyProfile,
									ECommonInputType::Gamepad,
									KeyMapping
								);
							
							GamepadCollection->AddChildListData(KeyRemapDataObject);
						}
					}
				}
			}
			
		}
		
		ControlTabCollection->AddChildListData(GamepadCollection);
	}
	RegisteredOptionsTabCollections.Add(ControlTabCollection);
}

FString UOptionsDataRegistry::GetOverallQualityDefaultValue() const
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	FString OverallQualityDefaultValue = TEXT("0");
	if (Settings)
	{
		OverallQualityDefaultValue = LexToString(Settings->GetOverallScalabilityLevel());
	}
	return OverallQualityDefaultValue;
}

FString UOptionsDataRegistry::GetResolutionDefaultValue() const
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	FString ResolutionDefaultValue = TEXT("0.f");
	if (Settings)
	{
		ResolutionDefaultValue = LexToString(Settings->GetResolutionScaleNormalized());
	}
	return ResolutionDefaultValue;
}

FString UOptionsDataRegistry::GetGlobalIlluminationQualityDefaultValue() const
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	FString GlobalIlluminationQuality = TEXT("0");
	if (Settings)
	{
		GlobalIlluminationQuality = LexToString(Settings->GetGlobalIlluminationQuality());
	}
	return GlobalIlluminationQuality;
}

FString UOptionsDataRegistry::GetShadowQualityDefaultValue() const
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	FString ShadowQuality = TEXT("0");
	if (Settings)
	{
		ShadowQuality = LexToString(Settings->GetShadowQuality());
	}
	return ShadowQuality;
}

FString UOptionsDataRegistry::GetAntiAliasingQualityDefaultValue() const
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	FString AntiAliasingQuality = TEXT("0");
	if (Settings)
	{
		AntiAliasingQuality = LexToString(Settings->GetAntiAliasingQuality());
	}
	return AntiAliasingQuality;
}

FString UOptionsDataRegistry::GetViewDistanceQualityDefaultValue() const
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	FString ViewDistanceQuality = TEXT("0");
	if (Settings)
	{
		ViewDistanceQuality = LexToString(Settings->GetViewDistanceQuality());
	}
	return ViewDistanceQuality;
}

FString UOptionsDataRegistry::GetTextureQualityDefaultValue() const
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	FString TextureQuality = TEXT("0");
	if (Settings)
	{
		TextureQuality = LexToString(Settings->GetTextureQuality());
	}
	return TextureQuality;
}

FString UOptionsDataRegistry::GetVisualEffectQualityDefaultValue() const
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	FString VisualEffectQuality = TEXT("0");
	if (Settings)
	{
		VisualEffectQuality = LexToString(Settings->GetVisualEffectQuality());
	}
	return VisualEffectQuality;
}

FString UOptionsDataRegistry::GetReflectionQualityDefaultValue() const
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	FString ReflectionQuality = TEXT("0");
	if (Settings)
	{
		ReflectionQuality = LexToString(Settings->GetReflectionQuality());
	}
	return ReflectionQuality;
}

FString UOptionsDataRegistry::GetPostProcessingQualityDefaultValue() const
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	FString PostProcessingQuality = TEXT("0");
	if (Settings)
	{
		PostProcessingQuality = LexToString(Settings->GetPostProcessingQuality());
	}
	return PostProcessingQuality;
}

FString UOptionsDataRegistry::GetVerticalSyncDefaultValue() const
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	FString VerticalSync = TEXT("0");
	if (Settings)
	{
		VerticalSync = LexToString(Settings->IsVSyncEnabled());
	}
	return VerticalSync;
}

FString UOptionsDataRegistry::GetFrameRateLimitDefaultValue() const
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	FString FrameRateLimit = TEXT("0");
	if (Settings)
	{
		FrameRateLimit = LexToString(Settings->GetFrameRateLimit());
	}
	return FrameRateLimit;
}
