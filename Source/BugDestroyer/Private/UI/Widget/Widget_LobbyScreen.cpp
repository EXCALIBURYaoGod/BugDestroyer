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
	// 1. 处理按钮可见性 (只有服务器房主能看到)
	if (CommonButton_StartGame)
	{
		// GetOwningPlayer 检查是否拥有权限 (HasAuthority)
		// 注意：UI 永远归 Client 所有，但我们需要检查这个 Client 是否也是 Host
		APlayerController* PC = GetOwningPlayer();
		if (PC && PC->HasAuthority()) 
		{
			CommonButton_StartGame->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			// 客户端隐藏按钮
			CommonButton_StartGame->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 2. 绑定 GameState 数据
	AGameStateBase* GS = UGameplayStatics::GetGameState(this);
	LobbyGameState = Cast<ALobbyGameState>(GS);

	if (LobbyGameState.IsValid())
	{
		// 绑定委托
		LobbyGameState->OnPlayerCountChanged.AddUniqueDynamic(this, &ThisClass::UpdatePlayerCount);
        
		// 第一次手动更新 (防止进房间时 UI 是空的)
		UpdatePlayerCount(LobbyGameState->CurrentPlayerCount);
	}
	
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

