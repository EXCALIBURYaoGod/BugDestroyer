// //Copyrights @FpsLuping all reserved


#include "Controllers/LobbyPlayerController.h"

#include "GameMode/LobbyGameMode.h"
#include "Subsystems/BugUISubsystem.h"
#include "Widget/Widget_PrimaryLayout.h"

void ALobbyPlayerController::RequestStartGameToUI()
{
	RPC_RequestStartGame();
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UBugUISubsystem* BugUISubsystem = UBugUISubsystem::Get(this))
	{
		if (UWidget_PrimaryLayout* Layout = BugUISubsystem->GetCreatedPrimaryLayout())
		{
			Layout->RemoveFromParent();
			BugUISubsystem->UnRegisterCreatedPrimaryLayoutWidget(Layout);
		}
	}
	Super::EndPlay(EndPlayReason);
	
}

void ALobbyPlayerController::RPC_RequestStartGame_Implementation()
{	

 	if (ALobbyGameMode* GM = GetWorld()->GetAuthGameMode<ALobbyGameMode>())
 	{
 		GM->ServerStartGame();
 	}
}


