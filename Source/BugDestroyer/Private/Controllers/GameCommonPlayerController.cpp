// Copyright @FpsLuping all reserved


#include "Controllers/GameCommonPlayerController.h"

#include "BugGameplayTags.h"
#include "BugUIFunctionLibrary.h"
#include "DebugHelper.h"
#include "Blueprint/UserWidget.h"
#include "Character/BugCharacter.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameState.h"
#include "GameMode/CommonGameMode.h"
#include "Subsystems/BugUISubsystem.h"
#include "Widget/Widget_MatchCooldownScreen.h"
#include "Widget/Widget_PrimaryLayout.h"


void AGameCommonPlayerController::ClientSetMatchState_Implementation(FName NewState)
{
	MatchState = NewState;
	
	if (IsLocalPlayerController())
	{
		UBugUISubsystem* BugUISubsystem = UBugUISubsystem::Get(this);
		if (BugUISubsystem)
		{
			UUserWidget* PrimaryLayout = BugUISubsystem->GetCreatedPrimaryLayout();
			if (!PrimaryLayout)
			{
				CreatePrimaryLayout();
			}
		}
        
        if (MatchState == MatchState::WaitingToStart)
        {
        	PushMatchBeforeStartScreen(); 
        }
        else if (MatchState == MatchState::InProgress)
        {
        	
        }
        else if (MatchState == MatchState::Cooldown)
        {
	        HandleMatchCooldownState();
        }
	}
	
	
}

void AGameCommonPlayerController::Client_ShowMatchCooldown_Implementation(const FText& WinnerNames)
{
	PushMatchCooldownScreen([WinnerNames](UWidget_MatchCooldownScreen* Screen)
	{
		if (Screen)
		{
			Screen->OnWinnerNameChangedCallback(WinnerNames);
		}
	});
}

void AGameCommonPlayerController::ShowSniperScopeWidget(bool bIsShow)
{
	if (bIsShow)
	{
		TSoftClassPtr<UWidget_ActivatableBase> ScopeClass = UBugUIFunctionLibrary::GetSoftWidgetClassByTag(BugGameplayTags::Bug_Widget_SniperScopeScreen);
		CachedSniperScopeWidget = UBugUISubsystem::Get(this)->PushSoftWidgetToStack(
			BugGameplayTags::Bug_WidgetStack_Modal, 
			ScopeClass
		);
	}
	else
	{
		if (CachedSniperScopeWidget)
		{
			CachedSniperScopeWidget->DeactivateWidget();
		}
	}
}

void AGameCommonPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	if (IsLocalPlayerController())
	{
		InitUIWithCharacter(aPawn);
		UBugUISubsystem::Get(this)->NotifyPawnResubscribed(aPawn);
	}
}

void AGameCommonPlayerController::AcknowledgePossession(APawn* aPawn)
{
	Super::AcknowledgePossession(aPawn);
	if (IsLocalPlayerController())
	{
		InitUIWithCharacter(aPawn);
		UBugUISubsystem::Get(this)->NotifyPawnResubscribed(aPawn);
	}
}

void AGameCommonPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	GetWorldTimerManager().SetTimerForNextTick([this]()
	{
		if (AGameState* GS = GetWorld()->GetGameState<AGameState>())
		{
			ClientSetMatchState_Implementation(GS->GetMatchState());
		}
	});
}

void AGameCommonPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UBugUISubsystem* BugUISubsystem = UBugUISubsystem::Get(this))
	{
		if (UWidget_PrimaryLayout* Layout = BugUISubsystem->GetCreatedPrimaryLayout())
		{
			Layout->RemoveFromParent();
			BugUISubsystem->UnRegisterCreatedPrimaryLayoutWidget(Layout);
		}
	}
	Super::EndPlay(EndPlayReason);
}


void AGameCommonPlayerController::InitUIWithCharacter(APawn* InPawn)
{

	UBugUISubsystem* BugUISubsystem = UBugUISubsystem::Get(this);
	UWidget_PrimaryLayout* Layout = BugUISubsystem->GetCreatedPrimaryLayout();
    
	if (Layout)
	{
		if (IsLocalPlayerController())
		{
            if (!bCharacterScreenPushed)
            {
            	PushCharacterScreen(InPawn);
            }
			
		}

	}
	else
	{
		ABugCharacter* BugCharacter = Cast<ABugCharacter>(InPawn);
		if (BugCharacter)
		{
			CreatePrimaryLayout();
			PushCharacterScreen(InPawn);
		}
	}

}

void AGameCommonPlayerController::CreatePrimaryLayout()
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
		}
	}
}

void AGameCommonPlayerController::PushMatchBeforeStartScreen()
{
	UBugUISubsystem::Get(this)->PushSoftWidgetToStackAsync(
		BugGameplayTags::Bug_WidgetStack_GameHud, 
		UBugUIFunctionLibrary::GetSoftWidgetClassByTag(BugGameplayTags::Bug_Widget_MatchBeforeStartScreen)
	);
}

void AGameCommonPlayerController::PushCharacterScreen(APawn* InPawn)
{
	bCharacterScreenPushed = true;
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

void AGameCommonPlayerController::PushMatchCooldownScreen(
	TFunction<void(UWidget_MatchCooldownScreen*)> OnCreatedCallback)
{
	UBugUISubsystem::Get(this)->PushSoftWidgetToStackAsync(
		BugGameplayTags::Bug_WidgetStack_GameHud, 
		UBugUIFunctionLibrary::GetSoftWidgetClassByTag(BugGameplayTags::Bug_Widget_MatchCooldownScreen),
		[this, OnCreatedCallback](EAsyncPushWidgetState PushWidgetState, UWidget_ActivatableBase* PushedWidget)
		{
			if (PushWidgetState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				UWidget_MatchCooldownScreen* Screen = Cast<UWidget_MatchCooldownScreen>(PushedWidget);
				if (OnCreatedCallback && Screen)
				{
					OnCreatedCallback(Screen);
				}
			}
		}
	);
}

void AGameCommonPlayerController::HandleMatchCooldownState()
{

}

