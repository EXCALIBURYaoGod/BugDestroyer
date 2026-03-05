// //Copyrights @FpsLuping all reserved


#include "GameState/LobbyGameState.h"

#include "Net/UnrealNetwork.h"

ALobbyGameState::ALobbyGameState()
{
	CurrentPlayerCount = 0;
}

void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// 注册变量进行网络复制
	DOREPLIFETIME(ALobbyGameState, CurrentPlayerCount);
}

void ALobbyGameState::SetPlayerCount(int32 NewCount)
{
	if (HasAuthority()) // 只有服务器能修改
	{
		CurrentPlayerCount = NewCount;
		// 服务器自己也要触发委托来更新UI (因为服务器不运行 OnRep)
		OnPlayerCountChanged.Broadcast(CurrentPlayerCount); 
	}
}

void ALobbyGameState::OnRep_PlayerCount()
{
	// 客户端收到更新后，广播委托通知 UI
	OnPlayerCountChanged.Broadcast(CurrentPlayerCount);
}