// //Copyrights @FpsLuping all reserved


#include "Controllers/LobbyPlayerController.h"

#include "GameMode/LobbyGameMode.h"

void ALobbyPlayerController::RequestStartGameToUI()
{
	RPC_RequestStartGame();
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyPlayerController::RPC_RequestStartGame_Implementation()
{	
	// 此时代码在服务器运行
 	if (ALobbyGameMode* GM = GetWorld()->GetAuthGameMode<ALobbyGameMode>())
 	{
 		GM->ServerStartGame();
 	}
}


