// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget_ActivatableBase.h"
#include "CommonInputTypeEnum.h"

#include "Widget_KeyRemapScreen.generated.h"

class UCommonRichTextBlock;
class FKeyRemapScreenInputPreprocessor;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class BUGDESTROYER_API UWidget_KeyRemapScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()
	
public:
	void SetDesiredInputTypeToFilter(ECommonInputType InDesiredInputType);
	
	DECLARE_DELEGATE_OneParam(FOnKeyRemapScreenKeyPressedDelegate, const FKey& /*PressedKey*/);
	FOnKeyRemapScreenKeyPressedDelegate OnKeyRemapScreenKeyPressed;
	DECLARE_DELEGATE_OneParam(FOnKeyRemapScreenKeySelectedCanceledDelegate, const FString& /*CanceledReason*/);
	FOnKeyRemapScreenKeySelectedCanceledDelegate OnKeyRemapScreenKeySelectedCanceled;
	
protected:
	// begin UCommonActivatableWidget Interface
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	// end UCommonActivatableWidget Interface
	
private:
	void OnValidKeyPressedDetected(const FKey& PressedKey);
	void OnKeySelectedCancelled(const FString& CanceledReason);
	
	// Delay a tick to make sure the input key is captured properly before calling the PreDeactivateCallback and deactivating the widget
	void RequestDeactivateWidget(TFunction<void()> PreDeactivateCallback);
	
	//***** Bind Widgets *****//
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "ture"))
	UCommonRichTextBlock* CommonRichText_RemapMsg;
	//***** Bind Widgets *****//
	
	TSharedPtr<FKeyRemapScreenInputPreprocessor> CachedInputPreprocessor;
	
	ECommonInputType CachedDesiredInputType;
	
};
