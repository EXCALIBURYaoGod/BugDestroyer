// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Widget_MultiplayerScreen.h"

#include "CommonListView.h"
#include "DebugHelper.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Controllers/BugUIPlayerController.h"
#include "Subsystems/BugUISubsystem.h"
#include "Widget/Components/BugCommonListView.h"
#include "Widget/Options/DataObjects/ListDataObject_Session.h"

void UWidget_MultiplayerScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MS_OnCreateSessionCompleteDelegate.AddUniqueDynamic(this, &ThisClass::OnCreateSessionComplete);
		MultiplayerSessionsSubsystem->MS_OnFindSessionCompleteDelegate.AddUniqueDynamic(this, &ThisClass::OnFindSessionsComplete);
		if (!MultiplayerSessionsSubsystem->MS_OnJoinSessionCompleteDelegate.IsBound())
		{
			MultiplayerSessionsSubsystem->MS_OnJoinSessionCompleteDelegate.AddUniqueDynamic(this, &ThisClass::OnJoinSessionComplete);
		}
	}
	
}

void UWidget_MultiplayerScreen::HostGame(int32 NumPublicConnections, FString MatchType)
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UWidget_MultiplayerScreen::SearchGame()
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}





void UWidget_MultiplayerScreen::OnCreateSessionComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (APlayerController* PC = GetOwningPlayerController())
		{
			FInputModeGameOnly GameOnly;
			PC->SetInputMode(GameOnly);
			PC->bShowMouseCursor = false;
		}
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(FString("/Game/Maps/Lobby?listen"));
		}
	}
	else
	{
		// 可以在这里 UI 提示：创建失败
	}
}

void UWidget_MultiplayerScreen::OnFindSessionsComplete(const TArray<FSessionInfo>& SessionResults, bool bWasSuccessful)
{
	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		Debug::Print(FString::Printf(TEXT("%s: No sessions found"), *FString(__FUNCTION__)));
		return;
	}
	
	if (!CommonListView_SessionListView) return;
	CommonListView_SessionListView->ClearListItems();
	for (auto Result : SessionResults)
	{
		// 创建包装对象
		UListDataObject_Session* NewEntry = NewObject<UListDataObject_Session>(this);
		NewEntry->SessionInfo = Result;
		NewEntry->InitDataObject();

		CommonListView_SessionListView->AddItem(NewEntry);
	}
	
	if (CommonListView_SessionListView->GetNumItems() != 0)
	{
		CommonListView_SessionListView->NavigateToIndex(0);
		CommonListView_SessionListView->SetSelectedIndex(0);
	}
	
}

void UWidget_MultiplayerScreen::OnJoinSessionComplete(bool bWasSuccessful, FString Address)
{
	if (bWasSuccessful && Address != FString(""))
	{
		Debug::Print(FString::Printf(TEXT("Joining Session Success: %s"), *Address));
		if (APlayerController* PC = GetOwningPlayer())
		{
			FInputModeGameOnly GameOnly;
			PC->SetInputMode(GameOnly);
			PC->bShowMouseCursor = false;
			DeactivateWidget(); 
			PC->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
}


