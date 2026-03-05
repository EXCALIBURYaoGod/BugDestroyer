// //Copyrights @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LobbyGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerCountChanged, int32, NewCount);

/**
 *	大厅游戏状态类，存储和同步玩家数量
 */
UCLASS()
class BUGDESTROYER_API ALobbyGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	
	ALobbyGameState();
	
	// 必须要重写这个函数来注册同步变量
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// 当前玩家数量 (ReplicatedUsing 表示当客户端收到这个值变化时，自动调用 OnRep_PlayerCount)
	UPROPERTY(ReplicatedUsing = OnRep_PlayerCount, BlueprintReadOnly, Category = "Lobby")
	int32 CurrentPlayerCount;
	
	UFUNCTION()
	void OnRep_PlayerCount();
	
	// 供 UI 绑定的委托
	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnPlayerCountChanged OnPlayerCountChanged;
	
	// 供 GameMode 调用的辅助函数
	void SetPlayerCount(int32 NewCount);

};
