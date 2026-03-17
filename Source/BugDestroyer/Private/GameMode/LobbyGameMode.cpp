// //Copyrights @FpsLuping all reserved


#include "GameMode/LobbyGameMode.h"

#include "GameFramework/GameStateBase.h"
#include "GameState/LobbyGameState.h"

ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = true;
}

void ALobbyGameMode::PostLogin(class APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (ALobbyGameState* MyGS = GetGameState<ALobbyGameState>())
	{
		// NumTravelingPlayers 是 GameModeBase 自带的统计，也可以自己维护计数
		int32 Count = GetNumPlayers(); 
		MyGS->SetPlayerCount(Count);
	}
	
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	if (ALobbyGameState* MyGS = GetGameState<ALobbyGameState>())
	{
		int32 Count = GetNumPlayers() - 1; 
		MyGS->SetPlayerCount(FMath::Max(0, Count));
	}
	
}

void ALobbyGameMode::ServerStartGame()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel("/Game/Maps/TeamDeathMatch?listen");
	}
}