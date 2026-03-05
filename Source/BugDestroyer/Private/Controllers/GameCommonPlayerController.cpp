// Copyright @FpsLuping all reserved


#include "Controllers/GameCommonPlayerController.h"

#include "BugGameplayTags.h"
#include "BugUIFunctionLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Character/BugCharacter.h"
#include "Subsystems/BugUISubsystem.h"
#include "Widget/Widget_PrimaryLayout.h"


void AGameCommonPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	if (IsLocalPlayerController())
	{
		InitUIWithCharacter(aPawn);
	}
}

void AGameCommonPlayerController::AcknowledgePossession(APawn* aPawn)
{
	Super::AcknowledgePossession(aPawn);
	if (IsLocalPlayerController())
	{
		InitUIWithCharacter(aPawn);
	}
}

void AGameCommonPlayerController::InitUIWithCharacter(APawn* InPawn)
{
	// 1. 尝试从指针或子系统获取现有的 UI
	UBugUISubsystem* BugUISubsystem = UBugUISubsystem::Get(this);
	UWidget_PrimaryLayout* Layout = BugUISubsystem->GetCreatedPrimaryLayout();
    
	if (Layout)
	{
		BugUISubsystem->NotifyPawnResubscribed(InPawn);
	}
	else
	{
		ABugCharacter* BugCharacter = Cast<ABugCharacter>(InPawn);
		if (BugCharacter)
		{
			CreateUI(InPawn);
		}
	}

}

void AGameCommonPlayerController::CreateUI(APawn* InPawn)
{
	if (!IsLocalPlayerController()) 
	{
		return;
	}
	
	if (GamePrimaryLayout)
	{
		UWidget_PrimaryLayout* PrimaryLayoutWidget = CreateWidget<UWidget_PrimaryLayout>(this, GamePrimaryLayout);
		if (PrimaryLayoutWidget)
		{
			PrimaryLayoutWidget->AddToViewport();
			UBugUISubsystem::Get(this)->RegisterCreatedPrimaryLayoutWidget(PrimaryLayoutWidget);
			UBugUISubsystem::Get(this)->PushSoftWidgetToStackAsync(
				BugGameplayTags::Bug_WidgetStack_GameHud, 
				UBugUIFunctionLibrary::GetSoftWidgetClassByTag(BugGameplayTags::Bug_Widget_CharacterScreen),
				[this, InPawn](EAsyncPushWidgetState PushWidgetState, UWidget_ActivatableBase* PushedWidget)
					{
						if (PushWidgetState == EAsyncPushWidgetState::OnCreatedBeforePush)
						{
							UBugUISubsystem* BugUISubsystem = UBugUISubsystem::Get(this);
							BugUISubsystem->NotifyPawnResubscribed(InPawn);
						}
					}
				);
		}
	}
}
