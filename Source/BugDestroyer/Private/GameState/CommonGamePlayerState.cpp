// //Copyrights @FpsLuping all reserved


#include "GameState/CommonGamePlayerState.h"

#include "DebugHelper.h"
#include "Controllers/GameCommonPlayerController.h"
#include "Net/UnrealNetwork.h"


void ACommonGamePlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACommonGamePlayerState, Defeats);
}

void ACommonGamePlayerState::Reset()
{
	Super::Reset();
	Defeats = 0;
}

void ACommonGamePlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	OnScoreChanged.Broadcast(GetScore());
}

void ACommonGamePlayerState::OnRep_Defeats()
{
	OnDefeatsChanged.Broadcast(GetDefeats());
}

void ACommonGamePlayerState::AddToScore(float InScore)
{
	if (HasAuthority())
	{
		SetScore(GetScore() + InScore);
		OnScoreChanged.Broadcast(GetScore());
	}
	
}

void ACommonGamePlayerState::AddToDefeats(int32 InDefeats)
{
	if (HasAuthority())
	{
		SetDefeats(GetDefeats() + InDefeats);
		OnDefeatsChanged.Broadcast(InDefeats);
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


