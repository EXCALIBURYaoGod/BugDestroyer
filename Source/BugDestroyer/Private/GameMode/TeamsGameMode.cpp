// Copyright @FpsLuping all reserved


#include "GameMode/TeamsGameMode.h"

#include "GameState/CommonGamePlayerState.h"
#include "GameState/CommonGameState.h"
#include "Kismet/GameplayStatics.h"

void ATeamsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (ACommonGameState* CommonGameState = Cast<ACommonGameState>(UGameplayStatics::GetGameState(this)))
	{
		ACommonGamePlayerState* CommonGamePlayerState = NewPlayer->GetPlayerState<ACommonGamePlayerState>();
		if (CommonGamePlayerState && CommonGamePlayerState->GetTeam() == ETeam::ET_NoneTeam)
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
	if (ACommonGameState* CommonGameState = Cast<ACommonGameState>(UGameplayStatics::GetGameState(this)))
	{
		for (auto _PlayerState : CommonGameState->PlayerArray)
		{
			ACommonGamePlayerState* CommonGamePlayerState = Cast<ACommonGamePlayerState>(_PlayerState);
			if (CommonGamePlayerState && CommonGamePlayerState->GetTeam() == ETeam::ET_NoneTeam)
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



