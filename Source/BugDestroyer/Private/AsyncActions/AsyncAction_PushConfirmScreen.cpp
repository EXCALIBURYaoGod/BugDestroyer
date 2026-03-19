// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncActions/AsyncAction_PushConfirmScreen.h"

#include "UI/Subsystems/BugUISubsystem.h"

UAsyncAction_PushConfirmScreen* UAsyncAction_PushConfirmScreen::PushConfirmScreen(const UObject* WorldContextObject, 
	APlayerController* OwningPlayerController, EConfirmScreenType ConfirmScreenType, FText InScreenTitle, FText InScreenMsg)
{
	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			UAsyncAction_PushConfirmScreen* Node = NewObject<UAsyncAction_PushConfirmScreen>();
			Node->CachedOwningWorld = World;
			Node->CachedOwningPC = OwningPlayerController;
			Node->CachedConfirmScreenType = ConfirmScreenType;
			Node->CachedScreenTitle = InScreenTitle;
			Node->CachedScreenMsg = InScreenMsg;
			
			Node->RegisterWithGameInstance(World);
			return Node;
		}
	}
	
	return nullptr;
}

void UAsyncAction_PushConfirmScreen::Activate()
{
	UBugUISubsystem* BugUISubsystem = UBugUISubsystem::Get(CachedOwningPC.Get());
	if (!BugUISubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("UAsyncAction_PushSoftWidget::Activate: 无法获取 BugUISubsystem！"));
		SetReadyToDestroy();
		return;
	}
	
	BugUISubsystem->PushConfirmScreenToModalStackAsync(
		CachedConfirmScreenType,
		CachedScreenTitle,
		CachedScreenMsg,
		[this](EConfirmScreenButtonType ClickedButtonType)
		{
			OnConfirmScreenButtonClicked.Broadcast(ClickedButtonType);
			
			SetReadyToDestroy();
		}
	);
}
