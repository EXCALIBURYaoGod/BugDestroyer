// Copyright @FpsLuping all reserved

#include "UI/Widget/Scoreboard/Widget_ScoreboardScreen.h"
#include "Widget/Components/BugCommonListView.h"
#include "GameFramework/GameStateBase.h"
#include "GameState/CommonGamePlayerState.h"
#include "UI/Widget/Scoreboard/ListEntries/Widget_ListEntry_ScoreboardRow.h"
#include "UI/Widget/Scoreboard/PlayerListData/ListDataObject_ScoreboardEntry.h"

void UWidget_ScoreboardScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	RefreshScoreboard();
	GetWorld()->GetTimerManager().SetTimer(RefreshTimerHandle, this, &ThisClass::RefreshScoreboard, 0.5f, true);
}

void UWidget_ScoreboardScreen::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	GetWorld()->GetTimerManager().ClearTimer(RefreshTimerHandle);
}

void UWidget_ScoreboardScreen::RefreshScoreboard()
{
	AGameStateBase* GS = GetWorld()->GetGameState();
	if (!GS || !ListView_Scoreboard) return;
	TArray<UListDataObject_ScoreboardEntry*> CurrentEntries;
	
	for (APlayerState* PS : GS->PlayerArray)
	{
		ACommonGamePlayerState* CommonPS = Cast<ACommonGamePlayerState>(PS);
		if (!CommonPS) continue;
		
		UListDataObject_ScoreboardEntry* EntryData = nullptr;
		if (UListDataObject_ScoreboardEntry** FoundEntry = ScoreboardDataMap.Find(CommonPS))
		{
			EntryData = *FoundEntry;
		}
		else
		{
			EntryData = NewObject<UListDataObject_ScoreboardEntry>(this);
			EntryData->TargetPlayerState = CommonPS;
			ScoreboardDataMap.Add(CommonPS, EntryData);
		}
		
		CurrentEntries.Add(EntryData);
	}
	
	CurrentEntries.Sort([](const UListDataObject_ScoreboardEntry& A, const UListDataObject_ScoreboardEntry& B) {
		int32 KillsA = A.TargetPlayerState.IsValid() ? A.TargetPlayerState->GetKills() : 0;
		int32 KillsB = B.TargetPlayerState.IsValid() ? B.TargetPlayerState->GetKills() : 0;
		return KillsA > KillsB;
	});
	
	ListView_Scoreboard->SetListItems(CurrentEntries);
	
	for (UUserWidget* DisplayedWidget : ListView_Scoreboard->GetDisplayedEntryWidgets())
	{
		if (UWidget_ListEntry_ScoreboardRow* RowWidget = Cast<UWidget_ListEntry_ScoreboardRow>(DisplayedWidget))
		{
			RowWidget->RefreshRowUI();
		}
	}
}