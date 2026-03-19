// Copyright @FpsLuping all reserved


#include "GameState/CommonGameState.h"

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
	
}

void ACommonGameState::Multicast_BroadcastKillMessage_Implementation(const FString& KillerName,
	const FString& VictimName, const FString& WeaponName, bool bIsHeadshot)
{
	OnKillMessageBroadcast.Broadcast(KillerName, VictimName, WeaponName, bIsHeadshot);
}

void ACommonGameState::OnRep_RedTeamScore()
{
	
}

void ACommonGameState::OnRep_BlueTeamScore()
{
	
}

void ACommonGameState::BeginPlay()
{
	Super::BeginPlay();
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





