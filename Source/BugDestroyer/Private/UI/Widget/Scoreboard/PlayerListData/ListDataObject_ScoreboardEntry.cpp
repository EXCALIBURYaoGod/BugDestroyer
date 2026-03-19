// Copyright @FpsLuping all reserved

// ⚠️ 注意：请根据你上一轮修复的 Public/Private 路径结构，修改这行 include！
#include "UI/Widget/Scoreboard/PlayerListData/ListDataObject_ScoreboardEntry.h" 

#include "GameState/CommonGamePlayerState.h" // 引入你的自定义 PlayerState
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

FString UListDataObject_ScoreboardEntry::GetPlayerName() const
{
	if (TargetPlayerState.IsValid())
	{
		return TargetPlayerState->GetPlayerName();
	}
	return TEXT("Unknown Player");
}

int32 UListDataObject_ScoreboardEntry::GetKills() const
{
	if (TargetPlayerState.IsValid())
	{
		return TargetPlayerState->GetKills();
	}
	return 0;
}

int32 UListDataObject_ScoreboardEntry::GetDeaths() const
{
	// 调用你在 ACommonGamePlayerState 中定义的 GetDeaths()
	if (TargetPlayerState.IsValid())
	{
		return TargetPlayerState->GetDefeats();
	}
	return 0;
}

int32 UListDataObject_ScoreboardEntry::GetPing() const
{
	if (TargetPlayerState.IsValid())
	{
		// 虚幻 5 推荐使用 GetPingInMilliseconds() 获取真实的毫秒延迟
		// FMath::RoundToInt 负责把 float 四舍五入转换成 int32 整数展示
		return FMath::RoundToInt(TargetPlayerState->GetPingInMilliseconds());
	}
	return 999; // 如果断线或获取不到，显示 999 延迟
}

bool UListDataObject_ScoreboardEntry::IsLocalPlayer() const
{
	if (TargetPlayerState.IsValid() && GetWorld())
	{
		// 在客户端上，GetFirstPlayerController() 永远指向坐在电脑前的这个本地玩家
		if (APlayerController* LocalPC = GetWorld()->GetFirstPlayerController())
		{
			// 如果当前这行数据的 PlayerState 和本地玩家的 PlayerState 内存地址一致，说明就是自己
			return TargetPlayerState.Get() == LocalPC->PlayerState;
		}
	}
	return false;
}