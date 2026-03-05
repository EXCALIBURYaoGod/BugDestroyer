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
}

void ACommonGameState::BeginPlay()
{
	Super::BeginPlay();
}

void ACommonGameState::OnRep_MatchTime()
{
	int32 M, S;
	GetFormattedTime(M, S); 
	OnMatchTimeUpdated.Broadcast(M, S);
}


void ACommonGameState::GetFormattedTime(int32& Minutes, int32& Seconds) const
{
	if (MatchTime > 0)
	{
		Minutes = MatchTime / 60;   // 整除得到分钟，例如 300 / 60 = 5
		Seconds = MatchTime % 60;   // 取余得到秒数，例如 305 % 60 = 5
	}
	else
	{
		Minutes = 0;
		Seconds = 0;
	}
}


