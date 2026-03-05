// //Copyrights @FpsLuping all reserved


#include "GameMode/CommonGameMode.h"

#include "Character/BugCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "GameState/CommonGamePlayerState.h"
#include "GameState/CommonGameState.h"
#include "Kismet/GameplayStatics.h"

ACommonGameMode::ACommonGameMode()
{
	bDelayedStart = true;
}

void ACommonGameMode::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(MatchTimerHandle, this, &ACommonGameMode::UpdateMatchTime, 1.0f, true);
	
}

void ACommonGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds();
	}

}

void ACommonGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC)
		{
			ACommonGamePlayerState* PS = PC->GetPlayerState<ACommonGamePlayerState>();
			if (PS)
			{
				PS->Reset(); 
			}
		}
	}
}

void ACommonGameMode::PlayerEliminated(class ABugCharacter* VictimCharacter, AController* VictimController,
                                       class AController* AttackerController)
{
	
	if (ACommonGamePlayerState* AttackerPS = Cast<ACommonGamePlayerState>(AttackerController->PlayerState))
	{
		AttackerPS->AddToScore(1.f);
	}

	if (ACommonGamePlayerState* VictimPS = Cast<ACommonGamePlayerState>(VictimController->PlayerState))
	{
		VictimPS->AddToDefeats(1);
	}
	
	if (VictimCharacter)
	{
		VictimCharacter->EliminateCharacter();
	}
}

void ACommonGameMode::RequestRespawn(ACharacter* VictimCharacter, AController* VictimController)
{
	if (VictimCharacter)
	{
		VictimCharacter->Reset();
		VictimCharacter->Destroy();
	}
	if (VictimController)
	{
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), Actors);
		int32 Selection = FMath::RandRange(0, Actors.Num() - 1);
		RestartPlayerAtPlayerStart(VictimController, Actors[Selection]);
	}
	
}

void ACommonGameMode::UpdateMatchTime()
{
	ACommonGameState* GS = GetGameState<ACommonGameState>();
	if (GS && GS->GetMatchTime() > 0)
	{
		GS->SetMatchTime(GS->GetMatchTime() - 1);
		int32 M, S;
		GS->GetFormattedTime(M, S); 
		GS->OnMatchTimeUpdated.Broadcast(M, S);
		if (GS->GetMatchTime() <= 0)
		{
			GetWorldTimerManager().ClearTimer(MatchTimerHandle);
			//HandleMatchEnd();
		}
	}
}


