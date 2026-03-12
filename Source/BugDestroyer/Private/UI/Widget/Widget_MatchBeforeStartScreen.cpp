// Copyright @FpsLuping all reserved


#include "Widget/Widget_MatchBeforeStartScreen.h"

#include "GameState/CommonGameState.h"

void UWidget_MatchBeforeStartScreen::NativeOnActivated()
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

void UWidget_MatchBeforeStartScreen::NativeConstruct()
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

void UWidget_MatchBeforeStartScreen::OnWarmTimeChangedCallback(int32 InNewMinutes, int32 InNewSeconds)
{
	BP_OnWarmTimeUpdated(InNewMinutes, InNewSeconds);
}

void UWidget_MatchBeforeStartScreen::SetupGameStateBindings(ACommonGameState* GS)
{
	GS->OnWarmupTimeUpdated.AddUniqueDynamic(this, &ThisClass::OnWarmTimeChangedCallback);
	int32 M, S;
	int32 WarmupTime = GS->GetWarmupTime();
	M = WarmupTime / 60;
	S = WarmupTime % 60;
	OnWarmTimeChangedCallback(M, S);
}

void UWidget_MatchBeforeStartScreen::OnGameStateSet(AGameStateBase* GameStateBase)
{
	GetWorld()->GameStateSetEvent.RemoveAll(this);
	if (ACommonGameState* GS = Cast<ACommonGameState>(GameStateBase))
	{
		SetupGameStateBindings(GS);
	}
}
