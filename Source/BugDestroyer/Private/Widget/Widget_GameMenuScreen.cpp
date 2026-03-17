// Copyright @FpsLuping all reserved

#include "Widget/Widget_GameMenuScreen.h"

#include "BugGameplayTags.h"
#include "BugUIFunctionLibrary.h"
#include "CommonButtonBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Subsystems/BugUISubsystem.h"

void UWidget_GameMenuScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 绑定按钮点击事件
	if (Button_Resume)
	{
		Button_Resume->OnClicked().AddUObject(this, &ThisClass::OnResumeClicked);
	}
	if (Button_Options)
	{
		Button_Options->OnClicked().AddUObject(this, &ThisClass::OnOptionsClicked);
	}
	if (Button_LeaveMatch)
	{
		Button_LeaveMatch->OnClicked().AddUObject(this, &ThisClass::OnLeaveMatchClicked);
	}
	if (Button_Quit)
	{
		Button_Quit->OnClicked().AddUObject(this, &ThisClass::OnQuitClicked);
	}
}

void UWidget_GameMenuScreen::NativeOnActivated()
{
	Super::NativeOnActivated();

	// 激活时显示鼠标
	if (APlayerController* PC = GetOwningPlayerController())
	{
		PC->SetShowMouseCursor(true);
	}
}

void UWidget_GameMenuScreen::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	
	if (APlayerController* PC = GetOwningPlayerController())
	{
		PC->SetShowMouseCursor(false);
	}
}

TOptional<FUIInputConfig> UWidget_GameMenuScreen::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
}

void UWidget_GameMenuScreen::OnResumeClicked()
{
	DeactivateWidget();
}

void UWidget_GameMenuScreen::OnOptionsClicked()
{
	UBugUISubsystem::Get(GetOwningPlayerController())->PushSoftWidgetToStackAsync(
		BugGameplayTags::Bug_WidgetStack_GameMenu,
		UBugUIFunctionLibrary::GetSoftWidgetClassByTag(BugGameplayTags::Bug_Widget_OptionsScreen)
	);
}

void UWidget_GameMenuScreen::OnLeaveMatchClicked()
{
	if (APlayerController* PC = GetOwningPlayerController())
	{
		PC->SetShowMouseCursor(false);
		DeactivateWidget();
		PC->ClientTravel(TEXT("/Game/Maps/FrontendTestMap"), TRAVEL_Absolute);
	}
}

void UWidget_GameMenuScreen::OnQuitClicked()
{

	if (APlayerController* PC = GetOwningPlayerController())
	{
		UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, true);
	}
}
