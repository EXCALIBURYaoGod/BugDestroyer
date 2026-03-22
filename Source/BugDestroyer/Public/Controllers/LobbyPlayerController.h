// //Copyrights @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 大厅玩家控制器类，UI 无法直接调用 GameMode（因为 UI 在客户端，GameMode 在服务器），必须通过 PlayerController 的 RPC。
 */
UCLASS()
class BUGDESTROYER_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// 供 UI 调用的函数
	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void RequestStartGameToUI();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInputComponent() override;
	
	// 实际发送给服务器的 RPC
	UFUNCTION(Server, Reliable)
	void RPC_RequestStartGame();
	
	
	
};
