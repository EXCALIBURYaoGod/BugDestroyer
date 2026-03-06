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
#include "Subsystems/BugUISubsystem.h"

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
	/*for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PC = It->Get())
		{
			// 获取并结算经济（从 PlayerState 读取/写入）
			if (ACommonGamePlayerState* PS = PC->GetPlayerState<ACommonGamePlayerState>())
			{
				// PS->SaveCurrentLoadout(); // 记录当前血量或武器（如果是保枪）
			}
			
			if (APawn* OldPawn = PC->GetPawn())
			{
				OldPawn->SetActorEnableCollision(false);
				OldPawn->SetActorHiddenInGame(true);
				PC->UnPossess();
				PC->SetPawn(nullptr);
				OldPawn->Destroy();
			}

			RestartPlayer(PC); 
		}
	}*/
	/*UBugUISubsystem::Get(this)->PushSoftWidgetToStackAsync(
		BugGameplayTags::Bug_WidgetStack_GameHud, 
		UBugUIFunctionLibrary::GetSoftWidgetClassByTag(BugGameplayTags::Bug_Widget_MatchCooldownScreen),
		[this](EAsyncPushWidgetState PushWidgetState, UWidget_ActivatableBase* PushedWidget)
			{
				if (PushWidgetState == EAsyncPushWidgetState::OnCreatedBeforePush)
				{
					if (GameState)
					{
						TArray<ACommonGamePlayerState*> WinningPlayers;
						float MaxScoreDifference = -MAX_flt; 
						for (APlayerState* PS : GameState->PlayerArray)
						{
							if (ACommonGamePlayerState* CommonPS = Cast<ACommonGamePlayerState>(PS))
							{
								float CurrentScoreDiff = CommonPS->GetScore() - CommonPS->GetDefeats();
				
								if (CurrentScoreDiff > MaxScoreDifference)
								{
									MaxScoreDifference = CurrentScoreDiff;
									WinningPlayers.Empty();
									WinningPlayers.Add(CommonPS);
								}
								else if (CurrentScoreDiff == MaxScoreDifference)
								{
									WinningPlayers.Add(CommonPS);
								}
							}
						}
						TArray<FString> WinnerNames;
						for (ACommonGamePlayerState* Winner : WinningPlayers)
						{
							if (Winner)
							{
								WinnerNames.Add(Winner->GetPlayerName());
							}
						}
						FString JoinedNames = FString::Join(WinnerNames, TEXT("\n"));
						FText FinalText = FText::FromString(JoinedNames);
						if (ACommonGameState* GS = GetGameState<ACommonGameState>())
						{
							GS->OnWinnerNameUpdated.Broadcast(FinalText);
						}
					}
				}
			}
		);*/
	TArray<FString> WinnerNames;
	float MaxScoreDifference = -MAX_flt; 
	for (APlayerState* PS : GameState->PlayerArray)
	{
		if (ACommonGamePlayerState* CommonPS = Cast<ACommonGamePlayerState>(PS))
		{
			float CurrentScoreDiff = CommonPS->GetScore() - CommonPS->GetDefeats();
			if (CurrentScoreDiff > MaxScoreDifference)
			{
				MaxScoreDifference = CurrentScoreDiff;
				WinnerNames.Empty();
				WinnerNames.Add(CommonPS->GetPlayerName());
			}
			else if (FMath::IsNearlyEqual(CurrentScoreDiff, MaxScoreDifference))
			{
				WinnerNames.Add(CommonPS->GetPlayerName());
			}
		}
	}

	FText FinalText = FText::FromString(FString::Join(WinnerNames, TEXT("\n")));
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


