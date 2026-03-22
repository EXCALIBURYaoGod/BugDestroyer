// Copyright @FpsLuping all reserved


#include "GameState/CommonGameState.h"

#include "Controllers/GameCommonPlayerController.h"
#include "GameMode/CommonGameMode.h"
#include "Net/UnrealNetwork.h"

ACommonGameState::ACommonGameState()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACommonGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ACommonGameState, MatchTime);
	DOREPLIFETIME(ACommonGameState, WarmupTime);
	DOREPLIFETIME(ACommonGameState, CooldownTime);
	DOREPLIFETIME(ACommonGameState, WinnerTeam);
	
}

void ACommonGameState::Multicast_BroadcastKillMessage_Implementation(const FString& KillerName,
	const FString& VictimName, const FString& WeaponName, bool bIsHeadshot)
{
	OnKillMessageBroadcast.Broadcast(KillerName, VictimName, WeaponName, bIsHeadshot);
}

void ACommonGameState::AddToRedTeamScore()
{
	++RedTeamScore;
	OnTeamScoreUpdated.Broadcast(RedTeamScore, BlueTeamScore);
}

void ACommonGameState::AddToBlueTeamScore()
{
	++BlueTeamScore;
	OnTeamScoreUpdated.Broadcast(RedTeamScore, BlueTeamScore);
}

void ACommonGameState::OnRep_RedTeamScore()
{
	OnTeamScoreUpdated.Broadcast(RedTeamScore, BlueTeamScore);
}

void ACommonGameState::OnRep_BlueTeamScore()
{
	OnTeamScoreUpdated.Broadcast(RedTeamScore, BlueTeamScore);
}

void ACommonGameState::OnRep_WinnerTeam()
{
	OnWinnerTeamUpdated.Broadcast(WinnerTeam);
}

void ACommonGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ACommonGameState::OnRep_MatchState()
{
	Super::OnRep_MatchState();
	if (GetWorld())
	{

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			if (AGameCommonPlayerController* PC = Cast<AGameCommonPlayerController>(It->Get()))
			{
				if (PC->IsLocalPlayerController())
				{
					PC->OnMatchStateUpdated(MatchState);
				}
			}
		}
	}
}

void ACommonGameState::OnRep_MatchTime()
{
	int32 M, S;
	M = MatchTime / 60;
	S = MatchTime % 60;
	OnMatchTimeUpdated.Broadcast(M, S);
}

void ACommonGameState::OnRep_WarmupTime()
{
	int32 M = WarmupTime / 60;
	int32 S = WarmupTime % 60;
	OnWarmupTimeUpdated.Broadcast(M, S);
}

void ACommonGameState::OnRep_CooldownTime()
{
	int32 M = CooldownTime / 60;
	int32 S = CooldownTime % 60;
	OnCooldownTimeUpdated.Broadcast(M, S);
}





