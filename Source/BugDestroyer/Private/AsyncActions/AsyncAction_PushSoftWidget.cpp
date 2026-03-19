// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncActions/AsyncAction_PushSoftWidget.h"

#include "UI/Subsystems/BugUISubsystem.h"

UAsyncAction_PushSoftWidget* UAsyncAction_PushSoftWidget::PushSoftWidget(const UObject* WorldContextObject,
                                                                         APlayerController* OwningPlayerController, TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,
                                                                         FGameplayTag InWidgetStackTag, bool bFocusOnNewlyPushedWidget)
{
	checkf(!InSoftWidgetClass.IsNull(), TEXT("PushSoftWidgetToStack was passed a null soft widget class"))
	if (GEngine)
	{
		if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
		{
			UAsyncAction_PushSoftWidget* Node = NewObject<UAsyncAction_PushSoftWidget>();
			Node->CachedOwningWorld = World;
			Node->CachedOwningPC = OwningPlayerController;
			Node->CachedSoftWidgetClass = InSoftWidgetClass;
			Node->CachedWidgetStackTag = InWidgetStackTag;
			Node->bCachedFocusOnNewlyPushedWidget = bFocusOnNewlyPushedWidget;
			
			Node->RegisterWithGameInstance(World);
			return Node;
		}
	}
	
	return nullptr;
}

void UAsyncAction_PushSoftWidget::Activate()
{
	UBugUISubsystem* BugUISubsystem = UBugUISubsystem::Get(CachedOwningPC.Get());
	if (!BugUISubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("UAsyncAction_PushSoftWidget::Activate: 无法获取 BugUISubsystem！"));
		SetReadyToDestroy();
		return;
	}
	
	BugUISubsystem->PushSoftWidgetToStackAsync(CachedWidgetStackTag, CachedSoftWidgetClass,
		[this](EAsyncPushWidgetState InPushWidgetState, UWidget_ActivatableBase* InPushWidget)
		{
			switch (InPushWidgetState)
			{
				case EAsyncPushWidgetState::OnCreatedBeforePush:
					if (InPushWidget)
					{
						InPushWidget->SetOwningPlayer(CachedOwningPC.Get());
						OnWidgetCreatedBeforePush.Broadcast(InPushWidget);
					}
					break;
				case EAsyncPushWidgetState::AfterPush:
					AfterPush.Broadcast(InPushWidget);
					if (bCachedFocusOnNewlyPushedWidget && InPushWidget)
					{
						if (UWidget* WidgetToFocus = InPushWidget->GetDesiredFocusTarget())
						{
							WidgetToFocus->SetFocus();
						}
					}
					SetReadyToDestroy();
					break;
				default:
					break;
			}
		}
	);
}
