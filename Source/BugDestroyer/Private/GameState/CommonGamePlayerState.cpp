// //Copyrights @FpsLuping all reserved


#include "GameState/CommonGamePlayerState.h"

#include "DebugHelper.h"
#include "Controllers/GameCommonPlayerController.h"
#include "Net/UnrealNetwork.h"


void ACommonGamePlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACommonGamePlayerState, Defeats);
	DOREPLIFETIME(ACommonGamePlayerState, Kills);
	DOREPLIFETIME(ACommonGamePlayerState, Team);
}

void ACommonGamePlayerState::Reset()
{
	Super::Reset();
	Defeats = 0;
}

void ACommonGamePlayerState::OnRep_Defeats()
{
	OnDefeatsChanged.Broadcast(GetDefeats());
}

void ACommonGamePlayerState::OnRep_Kills()
{
	OnKillsChanged.Broadcast(GetKills());
}

void ACommonGamePlayerState::OnRep_Team()
{
	OnTeamUpdatedDelegate.Broadcast(Team);
}

void ACommonGamePlayerState::AddToDefeats(int32 InDefeats)
{
	if (HasAuthority())
	{
		SetDefeats(GetDefeats() + InDefeats);
		OnDefeatsChanged.Broadcast(InDefeats);
	}
}

void ACommonGamePlayerState::AddToKills(int32 InKills)
{
	if (HasAuthority())
	{
		SetKills(GetKills() + InKills);
		OnDefeatsChanged.Broadcast(InKills);
	}
}

void ACommonGamePlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	OnPawnSet.AddDynamic(this, &ThisClass::HandleOnPawnSet);
}

void ACommonGamePlayerState::HandleOnPawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	if (NewPawn)
	{
		GameCommonPC = Cast<AGameCommonPlayerController>(NewPawn->GetController());
	}
}

void ACommonGamePlayerState::SetTeam(const ETeam NewTeam)
{
	Team = NewTeam;
	if (HasAuthority())
	{
		OnTeamUpdatedDelegate.Broadcast(Team);
	} 
}


