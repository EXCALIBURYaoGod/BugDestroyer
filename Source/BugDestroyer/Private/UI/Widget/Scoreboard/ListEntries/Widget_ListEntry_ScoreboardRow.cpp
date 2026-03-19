// Copyright @FpsLuping all reserved

#include "UI/Widget/Scoreboard/ListEntries/Widget_ListEntry_ScoreboardRow.h"

#include "CommonTextBlock.h"
#include "UI/Widget/Scoreboard/PlayerListData/ListDataObject_ScoreboardEntry.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

void UWidget_ListEntry_ScoreboardRow::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	
	CachedScoreData = Cast<UListDataObject_ScoreboardEntry>(ListItemObject);
	
	if (CachedScoreData)
	{
		RefreshRowUI();
	}
}

void UWidget_ListEntry_ScoreboardRow::RefreshRowUI()
{
	if (!CachedScoreData) return;

	// 1. 设置基础文本
	CommonTextBlock_PlayerName->SetText(FText::FromString(CachedScoreData->GetPlayerName()));
	CommonTextBlock_Kills->SetText(FText::AsNumber(CachedScoreData->GetKills()));
	CommonTextBlock_Deaths->SetText(FText::AsNumber(CachedScoreData->GetDeaths()));

	// 2. 动态处理 Ping 的颜色
	int32 CurrentPing = CachedScoreData->GetPing();
	CommonTextBlock_Ping->SetText(FText::AsNumber(CurrentPing));
	
	if (CurrentPing < 80)
	{
		CommonTextBlock_Ping->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
	}
	else if (CurrentPing < 150)
	{
		CommonTextBlock_Ping->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
	}
	else
	{
		CommonTextBlock_Ping->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
	}

	// 3. 高亮本地玩家
	Border_SelfHighlight->SetVisibility(ESlateVisibility::HitTestInvisible);

	if (CachedScoreData->IsLocalPlayer())
	{
		Border_SelfHighlight->SetBrushColor(FLinearColor(1.0f, 0.6f, 0.1f, 0.8f)); 
	}
	else
	{
		Border_SelfHighlight->SetBrushColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.5f)); 
	}
}