// //Copyrights @FpsLuping all reserved


#include "Widget/Widget_LobbyScreen.h"

#include "CommonTextBlock.h"
#include "DebugHelper.h"
#include "ICommonInputModule.h"
#include "Controllers/LobbyPlayerController.h"
#include "GameState/LobbyGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/Components/BugCommonButtonBase.h"
#include "Input/CommonUIInputTypes.h"

void UWidget_LobbyScreen::NativeConstruct()
{
	Super::NativeConstruct();
	if (CommonButton_StartGame)
	{
		APlayerController* PC = GetOwningPlayer();
		if (PC && PC->HasAuthority()) 
		{
			CommonButton_StartGame->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			CommonButton_StartGame->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 2. 不要在这里直接获取，因为客户端此时 GameState 大概率为 null
	// 开启一个每 0.1 秒执行一次的循环定时器，去尝试获取 GameState
	GetWorld()->GetTimerManager().SetTimer(WaitGameStateTimer, this, &ThisClass::TryBindGameState, 0.1f, true);
	
}

void UWidget_LobbyScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (CommonButton_StartGame)
	{
		CommonButton_StartGame->SetButtonText(FText::FromString(TEXT("StartGame")));
	}
	RegisterUIActionBinding(
		FBindUIActionArgs(
			CustomBackDataTableRowHandle,
			false,
			FSimpleDelegate::CreateUObject(this, &ThisClass::HandleBackAction))
		);
}

bool UWidget_LobbyScreen::NativeOnHandleBackAction()
{
	FInputModeGameOnly InputMode;
	GetOwningPlayerController()->SetInputMode(InputMode);
	
	GetOwningPlayerController()->bShowMouseCursor = false;
	return Super::NativeOnHandleBackAction();
}


void UWidget_LobbyScreen::UpdatePlayerCount(int32 NewCount)
{
	if (CommonText_PlayerCount)
	{
		FString CountStr = FString::Printf(TEXT("Players: %d"), NewCount);
		CommonText_PlayerCount->SetText(FText::FromString(CountStr));
	}
}

void UWidget_LobbyScreen::OnStartGameClicked()
{
	if (ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
	{
		DeactivateWidget();
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
		PC->RequestStartGameToUI();
	}
}

void UWidget_LobbyScreen::TryBindGameState()
{
	AGameStateBase* GS = UGameplayStatics::GetGameState(this);
	LobbyGameState = Cast<ALobbyGameState>(GS);
	
	if (LobbyGameState.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(WaitGameStateTimer);
		LobbyGameState->OnPlayerCountChanged.AddUniqueDynamic(this, &ThisClass::UpdatePlayerCount);
		UpdatePlayerCount(LobbyGameState->CurrentPlayerCount);
	}
}

