// Copyright @FpsLuping all reserved


#include "GameMode/TeamsGameMode.h"

#include "GameFramework/PlayerStart.h"
#include "GameState/CommonGamePlayerState.h"
#include "GameState/CommonGameState.h"
#include "Kismet/GameplayStatics.h"

void ATeamsGameMode::BeginPlay()
{
	Super::BeginPlay();
	if (ACommonGameState* GS = GetGameState<ACommonGameState>())
	{
		GS->SetMatchScore(MatchScore);
		GS->OnMatchScoreUpdated.Broadcast(MatchScore);
	}
}

void ATeamsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ATeamsGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	ACommonGameState* CommonGameState = Cast<ACommonGameState>(UGameplayStatics::GetGameState(this));
	ACommonGamePlayerState* CommonGamePlayerState = Exiting->GetPlayerState<ACommonGamePlayerState>();
	if (CommonGameState && CommonGamePlayerState)
	{
		if (CommonGameState->RedTeam.Contains(CommonGamePlayerState))
		{
			CommonGameState->RedTeam.Remove(CommonGamePlayerState);
		}
		if (CommonGameState->BlueTeam.Contains(CommonGamePlayerState))
		{
			CommonGameState->BlueTeam.Remove(CommonGamePlayerState);
		}
	}
}

AActor* ATeamsGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	FName TeamTag = NAME_None;
	
	if (ACommonGamePlayerState* PS = Player->GetPlayerState<ACommonGamePlayerState>())
	{
		if (PS->GetTeam() == ETeam::ET_RedTeam)
		{
			TeamTag = FName("RedTeam");
		}
		else if (PS->GetTeam() == ETeam::ET_BlueTeam)
		{
			TeamTag = FName("BlueTeam");
		}
	}
	
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), Actors);

	TArray<AActor*> TeamSpawns;
	for (AActor* Spawn : Actors)
	{
		if (APlayerStart* PlayerStart = Cast<APlayerStart>(Spawn))
		{
			/*Debug::Print(FString::Printf(TEXT("PlayerStartTag: %s"), *PlayerStart->PlayerStartTag.ToString()));*/
			if (PlayerStart->PlayerStartTag == TeamTag)
			{
				TeamSpawns.Add(PlayerStart);
			}
		}
	}

	if (TeamSpawns.Num() > 0)
	{
		int32 Selection = FMath::RandRange(0, TeamSpawns.Num() - 1);
		return TeamSpawns[Selection];
	}
	
	return Super::ChoosePlayerStart_Implementation(Player);
}

FString ATeamsGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
	const FString& Options, const FString& Portal)
{
	InitPlayerTeam(NewPlayerController);
	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

void ATeamsGameMode::InitPlayerTeam(AController* NewController)
{
	if (ACommonGameState* CommonGameState = GetGameState<ACommonGameState>())
	{
		if (ACommonGamePlayerState* CommonGamePlayerState = NewController->GetPlayerState<ACommonGamePlayerState>())
		{
			if (CommonGamePlayerState->GetTeam() == ETeam::ET_NoneTeam)
			{
				if (CommonGameState->BlueTeam.Num() >= CommonGameState->RedTeam.Num())
				{
					CommonGameState->RedTeam.AddUnique(CommonGamePlayerState);
					CommonGamePlayerState->SetTeam(ETeam::ET_RedTeam);
				}
				else
				{
					CommonGameState->BlueTeam.AddUnique(CommonGamePlayerState);
					CommonGamePlayerState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
		}
	}
}

void ATeamsGameMode::InitSeamlessTravelPlayer(AController* NewController)
{
	InitPlayerTeam(NewController);
	Super::InitSeamlessTravelPlayer(NewController);
}

void ATeamsGameMode::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();
}

void ATeamsGameMode::HandleMatchEnd()
{
	Super::HandleMatchEnd();
	ACommonGameState* CommonGameState = GetGameState<ACommonGameState>();
	if (CommonGameState)
	{
		if (CommonGameState->RedTeamScore > CommonGameState->BlueTeamScore)
		{
			CommonGameState->WinnerTeam = FText::FromString(TEXT("RED TEAM WIN"));
		}else if (CommonGameState->RedTeamScore == CommonGameState->BlueTeamScore)
		{
			CommonGameState->WinnerTeam = FText::FromString(TEXT("DRAW"));
		}
		else
		{
			CommonGameState->WinnerTeam = FText::FromString(TEXT("BLUE TEAM WIN"));
		}
		CommonGameState->OnWinnerTeamUpdated.Broadcast(CommonGameState->WinnerTeam);
	}
}

float ATeamsGameMode::CalculateActualDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	ACommonGamePlayerState* AttackerPlayerState = Attacker->GetPlayerState<ACommonGamePlayerState>();
	ACommonGamePlayerState* VictimPlayerState = Victim->GetPlayerState<ACommonGamePlayerState>();
	if (AttackerPlayerState == nullptr || VictimPlayerState == nullptr)
	{
		return Super::CalculateActualDamage(Attacker, Victim, BaseDamage);
	}
	if (AttackerPlayerState->GetTeam() == VictimPlayerState->GetTeam())
	{
		return 0.0f;
	}
	return Super::CalculateActualDamage(Attacker, Attacker, BaseDamage);
}

void ATeamsGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

void ATeamsGameMode::PlayerEliminated(class ABugCharacter* VictimCharacter, AController* VictimController,
	class AController* AttackerController, AActor* Causer, bool bHeadShot)
{
	Super::PlayerEliminated(VictimCharacter, VictimController, AttackerController, Causer, bHeadShot);
	if (ACommonGameState* CommonGameState = Cast<ACommonGameState>(UGameplayStatics::GetGameState(this)))
	{
		if (AttackerController)
		{
			if (ACommonGamePlayerState* AttackerPlayerState = AttackerController->GetPlayerState<ACommonGamePlayerState>())
			{
				if (AttackerPlayerState->GetTeam() == ETeam::ET_RedTeam)
				{
					CommonGameState->AddToRedTeamScore();
					if (CommonGameState->RedTeamScore >= CommonGameState->WinningScore)
					{
						HandleMatchEnd();
					}
				}else if (AttackerPlayerState->GetTeam() == ETeam::ET_BlueTeam)
				{
					CommonGameState->AddToBlueTeamScore();
					if (CommonGameState->BlueTeamScore >= CommonGameState->WinningScore)
					{
						HandleMatchEnd();
					}
				}
			}
		}
	}
}

void ATeamsGameMode::HandleCooldownMatchState()
{
	Super::HandleCooldownMatchState();
}



