// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_KeyRemapScreen.h"

#include "CommonInputSubsystem.h"
#include "Framework/Application/IInputProcessor.h"
#include "CommonRichTextBlock.h"
#include "CommonUITypes.h"
#include "DebugHelper.h"
#include "ICommonInputModule.h"


class FKeyRemapScreenInputPreprocessor : public IInputProcessor
{
	
public:
	FKeyRemapScreenInputPreprocessor(ECommonInputType InInputTypeListenTo, ULocalPlayer* InWeakPlayer)
		:CachedInputTypeListenTo(InInputTypeListenTo)
		,CachedWeakLocalPlayer(InWeakPlayer)
	{}
	
	DECLARE_DELEGATE_OneParam(FOnInputPreProcessorKeyPressedDelegate, const FKey& /*PressedKey*/);
	FOnInputPreProcessorKeyPressedDelegate OnInputPreProcessorKeyPressed;
	
	DECLARE_DELEGATE_OneParam(FOnInputPreProcessorKeySelectCanceledDelegate, const FString& /*CanceledReason*/);
	FOnInputPreProcessorKeySelectCanceledDelegate OnInputPreProcessorKeySelectCanceled;
	
protected:
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override
	{
		
	}
	
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		ProcessPressedKey(InKeyEvent.GetKey());
		
		return true;
	}
	
	virtual bool HandleMouseButtonDownEvent( FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		ProcessPressedKey(MouseEvent.GetEffectingButton());
		
		return true;
	}
	
	void ProcessPressedKey(const FKey& InPressedKey) const
	{
		
		// CommonUI会把DefaultClick键映射为LeftButtonMouse
		FCommonInputActionDataBase* InputActionData = ICommonInputModule::GetSettings().GetDefaultClickAction().GetRow<FCommonInputActionDataBase>(TEXT("CommonInputActionData"));
		check(InputActionData);
		const FKey DefaultClickKey = InputActionData->GetDefaultGamepadInputTypeInfo().GetKey();
		
		// 打开KeyRemapScreen的时候，如果切换到gamepad，会导致CurrentInputType变更，
		// 而CachedInputTypeListenTo还是在打开Screen的前一刻，所以这里要重新检测一下
		UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(CachedWeakLocalPlayer.Get());
		check(CommonInputSubsystem);
		ECommonInputType CurrentInputType = CommonInputSubsystem->GetCurrentInputType();
		
		switch (CachedInputTypeListenTo)
		{
			case ECommonInputType::MouseAndKeyboard:
				if (InPressedKey.IsGamepadKey() || CurrentInputType == ECommonInputType::Gamepad)
				{
					OnInputPreProcessorKeySelectCanceled.ExecuteIfBound(TEXT("Detected Gamepad Key pressed for keyboard inputs. Key Remap has been canceled. "));
					return;
				}
				break;
		case ECommonInputType::Gamepad:
				if (CurrentInputType == ECommonInputType::Gamepad && InPressedKey == EKeys::LeftMouseButton)
				{
					OnInputPreProcessorKeyPressed.ExecuteIfBound(DefaultClickKey);
					return;
				}
				if (!InPressedKey.IsGamepadKey())
				{
					OnInputPreProcessorKeySelectCanceled.ExecuteIfBound(TEXT("Detected non-gamepad inputs pressed for Gamepad Key. Key Remap has been canceled. "));
					return;
				}
				break;
			
			default:
				break;
		}
		
		OnInputPreProcessorKeyPressed.ExecuteIfBound(InPressedKey);
		
	}
	
	
private:
	ECommonInputType CachedInputTypeListenTo;
	TWeakObjectPtr<ULocalPlayer> CachedWeakLocalPlayer;
	
};

void UWidget_KeyRemapScreen::SetDesiredInputTypeToFilter(ECommonInputType InDesiredInputType)
{
	CachedDesiredInputType = InDesiredInputType;
}

void UWidget_KeyRemapScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	CachedInputPreprocessor = MakeShared<FKeyRemapScreenInputPreprocessor>(CachedDesiredInputType, GetOwningLocalPlayer());
	CachedInputPreprocessor->OnInputPreProcessorKeyPressed.BindUObject(this, &ThisClass::OnValidKeyPressedDetected);
	CachedInputPreprocessor->OnInputPreProcessorKeySelectCanceled.BindUObject(this, &ThisClass::OnKeySelectedCancelled);
	
	FSlateApplication::Get().RegisterInputPreProcessor(CachedInputPreprocessor, -1);
	
	FString InputDeviceName;

	switch (CachedDesiredInputType)
	{
		case ECommonInputType::MouseAndKeyboard:
		InputDeviceName = TEXT("Mouse & Keyboard");
		break;
		case ECommonInputType::Gamepad:
		InputDeviceName = TEXT("Gamepad");
		break;
		default:
		break;
	}
	
	const FString DisplayRichMsg =  FString::Printf(TEXT("<KeyRemapDefault>Press any</> <KeyRemapHighlight>%s</> <KeyRemapDefault>key.</>"), *InputDeviceName);
	CommonRichText_RemapMsg->SetText(FText::FromString(DisplayRichMsg));
	
	
}

void UWidget_KeyRemapScreen::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	if (CachedInputPreprocessor)
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(CachedInputPreprocessor);
		CachedInputPreprocessor.Reset();
	}
	
}

void UWidget_KeyRemapScreen::OnValidKeyPressedDetected(const FKey& PressedKey)
{
	RequestDeactivateWidget(
		[this, PressedKey]()
		{
			OnKeyRemapScreenKeyPressed.ExecuteIfBound(PressedKey);
		}
	);
}

void UWidget_KeyRemapScreen::OnKeySelectedCancelled(const FString& CanceledReason)
{
	RequestDeactivateWidget(
		[this, CanceledReason]()
		{
			OnKeyRemapScreenKeySelectedCanceled.ExecuteIfBound(CanceledReason);
		}
	);
}

void UWidget_KeyRemapScreen::RequestDeactivateWidget(TFunction<void()> PreDeactivateCallback)
{
	// Delay a tick to make sure the input is processed correctly
	FTSTicker::GetCoreTicker().AddTicker(
			FTickerDelegate::CreateLambda(
				[this, PreDeactivateCallback](float DeltaTime)->bool
				{
					PreDeactivateCallback();
					DeactivateWidget();
					return false;
				}
			)
		);
	
}
