// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Options/ListEntries/Widget_ListEntry_SessionEntry.h"

#include "CommonTextBlock.h"
#include "DebugHelper.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Widget/Options/DataObjects/ListDataObject_Session.h"

void UWidget_ListEntry_SessionEntry::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);
	CachedSessionData = Cast<UListDataObject_Session>(InOwningListDataObject);
	if (CachedSessionData)
	{
		CommonText_ServerName->SetText(CachedSessionData->GetDataDisplayName());
		CommonText_Index->SetText(FText::FromString(FString::FromInt(CachedSessionData->SessionInfo.InternalIndex)));
		CommonText_Ping->SetText(FText::FromString(FString::FromInt(CachedSessionData->SessionInfo.Ping)));;
		
		int32 CurrentPlayers = CachedSessionData->SessionInfo.CurrentPlayers;
		int32 MaxPlayers = CachedSessionData->SessionInfo.MaxPlayers;
		FText PlayersText = FText::Format(
			NSLOCTEXT("Multiplayer", "PlayerCountFormat", "{0}/{1}"),
			FText::AsNumber(CurrentPlayers),
			FText::AsNumber(MaxPlayers)
		);
		CommonText_CurrentMaxPlayers->SetText(PlayersText);
	}
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}
}

void UWidget_ListEntry_SessionEntry::JoinGameByIndex()
{
	if (MultiplayerSessionsSubsystem)
	{
		if (CachedSessionData)
		{
			int32 Index = CachedSessionData->SessionInfo.InternalIndex;
			MultiplayerSessionsSubsystem->JoinSessionByUIIndex(Index);
			Debug::Print(FString::Printf(TEXT("Joining Server :%d"), Index));
		}
		
	}
}

