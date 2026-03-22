// Copyright @FpsLuping all reserved


#include "Widget/Widget_MatchCooldownScreen.h"

#include "GameState/CommonGameState.h"

void UWidget_MatchCooldownScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	APlayerController* PC = GetOwningPlayerController();
	if (PC)
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}
}

void UWidget_MatchCooldownScreen::NativeConstruct()
{
	Super::NativeConstruct();
	if (ACommonGameState* GS = GetWorld()->GetGameState<ACommonGameState>())
	{
		SetupGameStateBindings(GS);
	}
	else
	{
		GetWorld()->GameStateSetEvent.AddUObject(this, &ThisClass::OnGameStateSet);
	}
}

void UWidget_MatchCooldownScreen::OnCooldownTimeChangedCallback(int32 InNewMinutes, int32 InNewSeconds)
{
	BP_OnCooldownTimeUpdated(InNewMinutes, InNewSeconds);
}

void UWidget_MatchCooldownScreen::OnWinnerTeamChangedCallback(const FText& WinnerTeam)
{
	BP_OnWinnerTeamUpdated(WinnerTeam);
}

void UWidget_MatchCooldownScreen::OnWinnerNameChangedCallback(const FText& WinnerNames)
{
	BP_OnWinnerNameUpdated(WinnerNames);
}

void UWidget_MatchCooldownScreen::SetupGameStateBindings(ACommonGameState* GS)
{
	GS->OnCooldownTimeUpdated.AddUniqueDynamic(this, &ThisClass::OnCooldownTimeChangedCallback);
	GS->OnWinnerTeamUpdated.AddUniqueDynamic(this, &ThisClass::OnWinnerTeamChangedCallback);
	int32 M, S;
	int32 CooldownTime = GS->GetCooldownTime();
	M = CooldownTime / 60;
	S = CooldownTime % 60;
	OnCooldownTimeChangedCallback(M, S);
	OnWinnerTeamChangedCallback(GS->WinnerTeam);
}

void UWidget_MatchCooldownScreen::OnGameStateSet(AGameStateBase* GameStateBase)
{
	GetWorld()->GameStateSetEvent.RemoveAll(this);
	if (ACommonGameState* GS = Cast<ACommonGameState>(GameStateBase))
	{
		SetupGameStateBindings(GS);
	}
}
