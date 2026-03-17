// Copyright @FpsLuping all reserved


#include "Controllers/GameCommonPlayerController.h"

#include "BugGameplayTags.h"
#include "BugUIFunctionLibrary.h"
#include "DebugHelper.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Blueprint/UserWidget.h"
#include "Character/BugCharacter.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "GameMode/CommonGameMode.h"
#include "Subsystems/BugUISubsystem.h"
#include "Weapons/Weapon.h"
#include "Widget/Widget_MatchCooldownScreen.h"
#include "Widget/Widget_PrimaryLayout.h"

AGameCommonPlayerController::AGameCommonPlayerController()
{
}

void AGameCommonPlayerController::ClientSetMatchState_Implementation(FName NewState)
{
	MatchState = NewState;
	
	if (IsLocalPlayerController())
	{
        
        if (MatchState == MatchState::WaitingToStart)
        {
        	PushMatchBeforeStartScreen(); 
        }
        else if (MatchState == MatchState::InProgress)
        {
        	if (!bCharacterScreenPushed && GetPawn())
        	{
        		PushCharacterScreen(GetPawn());
        	}
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

void AGameCommonPlayerController::BeginPlay()
{
	Super::BeginPlay();
	bCharacterScreenPushed = false;
	GetWorldTimerManager().SetTimer(TimeSyncTimer, this, &ThisClass::CheckTimeSync, 0.5f, true);
	if (IsLocalPlayerController())
	{
		GetWorldTimerManager().SetTimer(NetStatTimerHandle, this, &ThisClass::UpdateNetworkStats, 1.0f, true);
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
	if (IsLocalPlayerController())
	{
		CreatePrimaryLayout();
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
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

	if (!IsLocalPlayerController()) return;

	UBugUISubsystem* BugUISubsystem = UBugUISubsystem::Get(this);
	if (!BugUISubsystem) return;
	
	if (MatchState == MatchState::InProgress) 
	{
		if (BugUISubsystem->GetCreatedPrimaryLayout() && !bCharacterScreenPushed)
		{
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

void AGameCommonPlayerController::CheckTimeSync()
{
	ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	if (SyncCount > 5)
	{
		GetWorldTimerManager().SetTimer(TimeSyncTimer, this, &ThisClass::CheckTimeSync, 10.0f, true);
	}
	SyncCount++;
}

void AGameCommonPlayerController::ClientReportServerTime_Implementation(float ClientRequestTime,
                                                                        float TimeServerReceived)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - ClientRequestTime;
	float CurrentServerTime = TimeServerReceived + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

void AGameCommonPlayerController::ServerRequestServerTime_Implementation(float ClientRequestTime)
{
	ClientReportServerTime(ClientRequestTime, GetWorld()->GetTimeSeconds());
}

void AGameCommonPlayerController::UpdateNetworkStats()
{
	if (APlayerState* PS = PlayerState) 
	{
		CurrentPing = PS->GetPingInMilliseconds(); 
	}
	
	if (UNetDriver* NetDriver = GetWorld()->GetNetDriver())
	{
		UNetConnection* NetConn = NetDriver->ServerConnection; 
		
		if (NetConn)
		{
			float InLoss = NetConn->GetInLossPercentage().GetAvgLossPercentage();
			float OutLoss = NetConn->GetOutLossPercentage().GetAvgLossPercentage();
    
			PacketLossPercentage = FMath::Max(InLoss, OutLoss);
		}
	}
	
	bool bShouldWarn = (CurrentPing > 200.f || PacketLossPercentage > 5.f);
	if (bShouldWarn)
	{
		
	}
	if (bLastNetWarning != bShouldWarn)
	{
		bLastNetWarning = bShouldWarn;
		if (ABugCharacter* BugChar = Cast<ABugCharacter>(GetPawn()))
		{
			BugChar->OnNetWarning.Broadcast(bShouldWarn);
			ServerSetWeaponSSR(bShouldWarn);
		}
	}
}

void AGameCommonPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (ToggleMenuAction)
		{
			EnhancedInputComponent->BindAction(ToggleMenuAction, ETriggerEvent::Started, this, &ThisClass::ToggleGameMenu);
		}
	}
	
}

void AGameCommonPlayerController::ToggleGameMenu()
{
	if (UBugUISubsystem* BugUISubsystem = UBugUISubsystem::Get(this))
	{
		BugUISubsystem->PushSoftWidgetToStackAsync(
			BugGameplayTags::Bug_WidgetStack_GameMenu,
			UBugUIFunctionLibrary::GetSoftWidgetClassByTag(BugGameplayTags::Bug_Widget_GameMenuScreen)
		);
	}
}

void AGameCommonPlayerController::ServerSetWeaponSSR_Implementation(bool bEnableSSR)
{
	if (ABugCharacter* BugCharacter = Cast<ABugCharacter>(GetPawn()))
	{
		if (AWeapon* EquippedWeapon = BugCharacter->GetEquippedWeapon())
		{
			if (EquippedWeapon->bDefaultUseSSR)
			{
				EquippedWeapon->SetServerSideRewind(bEnableSSR);
			}
		}
		
	}
}
