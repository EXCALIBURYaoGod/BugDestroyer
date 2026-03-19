// //Copyrights @FpsLuping all reserved


#include "GameMode/CommonGameMode.h"

#include "BugGameplayTags.h"
#include "BugUIFunctionLibrary.h"
#include "DebugHelper.h"
#include "Character/BugCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "GameState/CommonGamePlayerState.h"
#include "GameState/CommonGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Controllers/GameCommonPlayerController.h"
#include "UI/Subsystems/BugUISubsystem.h"
#include "Weapons/Weapon.h"

namespace MatchState
{
	const FName Cooldown = FName(TEXT("Cooldown"));
}

ACommonGameMode::ACommonGameMode()
{
	bDelayedStart = true;
}

void ACommonGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	if (ACommonGameState* GS = GetGameState<ACommonGameState>())
	{
		GS->SetWarmupTime(WarmupTime);
		GS->SetMatchTime(MatchTime);
		GS->SetCooldownTime(CooldownTime);
	}
	if (MatchState == MatchState::WaitingToStart)
	{
		GetWorldTimerManager().SetTimer(WarmupTimerHandle, this, &ACommonGameMode::UpdateWarmupTime, 1.0f, true);
	}
	
}

void ACommonGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACommonGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

void ACommonGameMode::BroadcastElimAnnouncement(AController* VictimController, class AController* AttackerController, AActor* Causer, bool bHeadShot)
{
	FString KillerName = AttackerController ? AttackerController->PlayerState->GetPlayerName() : TEXT("Environment");
	FString VictimName = VictimController ? VictimController->PlayerState->GetPlayerName() : TEXT("Unknown");
	// TODO: 判断是否爆头
	FString WeaponName = TEXT("Generic"); 

	if (AWeapon* Weapon = Cast<AWeapon>(Causer))
	{
		EWeaponType CurrentWeaponType = Weapon->GetWeaponType(); 
		WeaponName = StaticEnum<EWeaponType>()->GetNameStringByValue(static_cast<int64>(CurrentWeaponType));
	}

	if (ACommonGameState* GS = GetGameState<ACommonGameState>())
	{
		GS->Multicast_BroadcastKillMessage(KillerName, VictimName, WeaponName, bHeadShot);
	}
}

void ACommonGameMode::PlayerEliminated(class ABugCharacter* VictimCharacter, AController* VictimController,
                                       class AController* AttackerController, AActor* Causer, bool bHeadShot)
{
	
	if (ACommonGamePlayerState* AttackerPS = Cast<ACommonGamePlayerState>(AttackerController->PlayerState))
	{
		AttackerPS->AddToKills(1);
	}

	if (ACommonGamePlayerState* VictimPS = Cast<ACommonGamePlayerState>(VictimController->PlayerState))
	{
		VictimPS->AddToDefeats(1);
	}
	
	BroadcastElimAnnouncement(VictimController, AttackerController, Causer, bHeadShot);
	
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

void ACommonGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (AGameCommonPlayerController* PC = Cast<AGameCommonPlayerController>(NewPlayer))
	{
		PC->ClientSetMatchState(MatchState);
		if (PC->IsLocalPlayerController())
		{
			if (ACommonGamePlayerState* PS = PC->GetPlayerState<ACommonGamePlayerState>())
            {
            	PS->SetPlayerName(TEXT("FpsLuping"));
            }
		}

	}
	
}

void ACommonGameMode::UpdateMatchTime()
{
	ACommonGameState* GS = GetGameState<ACommonGameState>();
	if (GS && GS->GetMatchTime() > 0)
	{
		GS->SetMatchTime(GS->GetMatchTime() - 1);
		int32 M, S;
		MatchTime = GS->GetMatchTime();
		M = MatchTime / 60;
		S = MatchTime % 60;
		GS->OnMatchTimeUpdated.Broadcast(M, S);
		if (GS->GetMatchTime() <= 0)
		{
			HandleMatchEnd();
		}
	}
}

void ACommonGameMode::UpdateWarmupTime()
{
	if (MatchState == MatchState::WaitingToStart)
	{
		if (ACommonGameState* GS = GetGameState<ACommonGameState>())
		{
			GS->SetWarmupTime(GS->GetWarmupTime() - 1);
			int32 TotalSeconds = GS->GetWarmupTime();
			int32 M = TotalSeconds / 60;
			int32 S = TotalSeconds % 60;
			GS->OnWarmupTimeUpdated.Broadcast(M, S);
			if (GS->GetWarmupTime() <= 0.f)
			{
				GetWorldTimerManager().ClearTimer(WarmupTimerHandle);
				StartMatch();
				GetWorldTimerManager().SetTimer(MatchTimerHandle, this, &ACommonGameMode::UpdateMatchTime, 1.0f, true);
			}
		}
	}
}

void ACommonGameMode::HandleCooldownMatchState()
{
	GetWorldTimerManager().ClearTimer(CooldownTimerHandle);
	RestartGame();
}

void ACommonGameMode::UpdateCooldownTime()
{
	if (MatchState == MatchState::Cooldown)
	{
		if (ACommonGameState* GS = GetGameState<ACommonGameState>())
		{
			GS->SetCooldownTime(GS->GetCooldownTime() - 1);
			int32 TotalSeconds = GS->GetCooldownTime();
			int32 M = TotalSeconds / 60;
			int32 S = TotalSeconds % 60;
			GS->OnCooldownTimeUpdated.Broadcast(M, S);
			if (GS->GetCooldownTime() <= 0)
			{
				HandleCooldownMatchState();
			}
		}
	}
}

void ACommonGameMode::HandleMatchEnd()
{
	GetWorldTimerManager().ClearTimer(MatchTimerHandle);
	SetMatchState(MatchState::Cooldown);
	TArray<FString> MVPNames;
	float MaxScoreDifference = -MAX_flt; 
	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (ACommonGamePlayerState* CommonPS = Cast<ACommonGamePlayerState>(PS))
		{
			float CurrentScoreDiff = CommonPS->GetKills() - CommonPS->GetDefeats();
			if (CurrentScoreDiff > MaxScoreDifference)
			{
				MaxScoreDifference = CurrentScoreDiff;
				MVPNames.Empty();
				MVPNames.Add(CommonPS->GetPlayerName());
			}
			else if (FMath::IsNearlyEqual(CurrentScoreDiff, MaxScoreDifference))
			{
				MVPNames.Add(CommonPS->GetPlayerName());
			}
		}
	}

	FText FinalText = FText::FromString(FString::Join(MVPNames, TEXT("\n")));
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (AGameCommonPlayerController* PC = Cast<AGameCommonPlayerController>(It->Get()))
		{
			PC->Client_ShowMatchCooldown(FinalText);
		}
	}
	GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &ACommonGameMode::UpdateCooldownTime, 1.0f, true);
}

void ACommonGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AGameCommonPlayerController* PC = Cast<AGameCommonPlayerController>(It->Get());
		if (PC)
		{
			PC->ClientSetMatchState(MatchState);
		}
	}
}


