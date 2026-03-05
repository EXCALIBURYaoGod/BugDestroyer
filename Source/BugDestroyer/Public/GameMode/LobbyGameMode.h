// //Copyrights @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 大厅游戏模式类，负责处理玩家进出，并统计人数
 */
UCLASS()
class BUGDESTROYER_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ALobbyGameMode();
	
	// begin AGameMode Interface
	virtual void PostLogin(class APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	// end AGameMode Interface
	
	// 执行开始游戏跳转
	void ServerStartGame();
	
};
