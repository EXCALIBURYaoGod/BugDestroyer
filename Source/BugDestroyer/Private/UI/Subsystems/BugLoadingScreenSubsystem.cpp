// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/BugLoadingScreenSubsystem.h"

#include "PreLoadScreenManager.h"
#include "Settings/BugLoadingScreenSettings.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/BugLoadingScrennInterface.h"

bool UBugLoadingScreenSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// check: 1、只在服务器创建 2、保证全局仅一个子类创建实例
	if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> FoundClasses;
		GetDerivedClasses(GetClass(), FoundClasses);
		return FoundClasses.IsEmpty();
	}
	return false;
}

void UBugLoadingScreenSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &ThisClass::OnMapPreLoaded);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::OnMapPostLoaded);
	
}

void UBugLoadingScreenSubsystem::Deinitialize()
{
	FCoreUObjectDelegates::PreLoadMapWithContext.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	
}

UWorld* UBugLoadingScreenSubsystem::GetTickableGameObjectWorld() const
{
	if (UGameInstance* OwningGameInstance = GetGameInstance())
	{
		return OwningGameInstance->GetWorld();
	}
	return FTickableGameObject::GetTickableGameObjectWorld();
}

void UBugLoadingScreenSubsystem::Tick(float DeltaTime)
{
	TryUpdateLoadingScreen();
}

ETickableTickType UBugLoadingScreenSubsystem::GetTickableTickType() const
{
	if (IsTemplate())
	{
		return ETickableTickType::Never;
	}
	return ETickableTickType::Conditional;
}

bool UBugLoadingScreenSubsystem::IsTickable() const
{
	return GetGameInstance() && GetGameInstance()->GetGameViewportClient();
}

TStatId UBugLoadingScreenSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UBugLoadingScreenSubsystem, STATGROUP_Tickables);
}

void UBugLoadingScreenSubsystem::OnMapPreLoaded(const FWorldContext& Context, const FString& MapName)
{
	if (Context.OwningGameInstance != GetGameInstance())
	{
		return;
	}
	
	SetTickableTickType(ETickableTickType::Conditional);
	
	bIsCurrentlyLoadingMap = true;
	
	TryUpdateLoadingScreen();
	
}

void UBugLoadingScreenSubsystem::OnMapPostLoaded(UWorld* LoadedWorld)
{
	if (LoadedWorld && LoadedWorld->GetGameInstance() == GetGameInstance())
	{
		bIsCurrentlyLoadingMap = false;
	}
}

void UBugLoadingScreenSubsystem::TryUpdateLoadingScreen()
{
	// check if there is any start up loading screen that's currently active
	if (IsPreLoadScreenActive())
	{
		return;
	}
	
	//check if we should show the loading screen
	if (ShouldShowLoadingScreen())
	{
		//try display the loading screen here
		TryDisplayLoadingScreenIfNone();
		NotifyLoadingScreenVisibilityChanged(true);
		OnLoadingReasonUpdated.Broadcast(CurrentLoadingReason);
		
	}
	else
	{
		TryRemoveLoadingScreen();
		HoldLoadingScreenStartUpTime = -1.f;
		NotifyLoadingScreenVisibilityChanged(false);
		SetTickableTickType(ETickableTickType::Never);
	}
	
}

bool UBugLoadingScreenSubsystem::IsPreLoadScreenActive() const
{
	if (FPreLoadScreenManager* PreLoadScreenManager = FPreLoadScreenManager::Get())
	{
		return PreLoadScreenManager->HasValidActivePreLoadScreen();
	}
	return false;
}

bool UBugLoadingScreenSubsystem::ShouldShowLoadingScreen()
{
	const UBugLoadingScreenSettings* LoadingScreenSettings = GetDefault<UBugLoadingScreenSettings>();

	if (GIsEditor && !LoadingScreenSettings->bShouldLoadingScreenInEditor)
	{
		return false;
	}
	
	//check if the objects in the world need a loading screen
	if (CheckTheNeedToShowLoadingScreen())
	{
		GetGameInstance()->GetGameViewportClient()->bDisableWorldRendering = true;
		return true;
	}
	
	CurrentLoadingReason = TEXT("Waiting for Texture Streaming");
	
	GetGameInstance()->GetGameViewportClient()->bDisableWorldRendering = false;
	
	const float CurrentTime = FPlatformTime::Seconds();

	if (HoldLoadingScreenStartUpTime < 0.f)
	{
		HoldLoadingScreenStartUpTime = CurrentTime;
	}
	
	const float ElapsedTime = CurrentTime - HoldLoadingScreenStartUpTime;

	if (ElapsedTime < LoadingScreenSettings->HoldLoadingScreenExtraSeconds)
	{
		return true;
	}
	
	return false;
}

bool UBugLoadingScreenSubsystem::CheckTheNeedToShowLoadingScreen()
{
	if (bIsCurrentlyLoadingMap)
	{
		CurrentLoadingReason = TEXT("Loading Level");
		return true;
	}
	UWorld* OwningWorld = GetGameInstance()->GetWorld();
	if (!OwningWorld)
	{
		CurrentLoadingReason = TEXT("Initializing World");
		return true;
	}

	if (!OwningWorld->HasBegunPlay())
	{
		CurrentLoadingReason = TEXT("Loading World");
		return true;
	}

	if (!OwningWorld->GetFirstPlayerController())
	{
		CurrentLoadingReason = TEXT("Loading PlayerController");
		return true;
	}
	
	//check if the game states, player states or player character, actor component are ready.
	
	
	return false;
	
}

void UBugLoadingScreenSubsystem::TryDisplayLoadingScreenIfNone()
{
	// If there's already active loading screen, return
	if (CachedCreatedLoadingScreenWidget)
	{
		return;
	}
	
	const UBugLoadingScreenSettings* LoadingScreenSettings = GetDefault<UBugLoadingScreenSettings>();
	TSubclassOf<UUserWidget> LoadedWidgetClass = LoadingScreenSettings->GetLoadingScreenWidgetClassChecked();
	UUserWidget* CreatedWidget = UUserWidget::CreateWidgetInstance(*GetGameInstance(), LoadedWidgetClass, NAME_None);
	
	check(CreatedWidget);
	CachedCreatedLoadingScreenWidget = CreatedWidget->TakeWidget();
	
	GetGameInstance()->GetGameViewportClient()->AddViewportWidgetContent(
			CachedCreatedLoadingScreenWidget.ToSharedRef(),
			1000
		);
	
	
}

void UBugLoadingScreenSubsystem::TryRemoveLoadingScreen()
{
	if (!CachedCreatedLoadingScreenWidget)
	{
		return;
	}
	
	GetGameInstance()->GetGameViewportClient()->RemoveViewportWidgetContent(CachedCreatedLoadingScreenWidget.ToSharedRef());
	CachedCreatedLoadingScreenWidget.Reset();
	
}

void UBugLoadingScreenSubsystem::NotifyLoadingScreenVisibilityChanged(bool bIsVisible)
{
	for (ULocalPlayer* ExistingLocalPlayer : GetGameInstance()->GetLocalPlayers())
	{
		if (!ExistingLocalPlayer)
		{
			continue;
		}
		if (APlayerController* PC = ExistingLocalPlayer->GetPlayerController(GetGameInstance()->GetWorld()))
		{
			//Query If the player controller implements the interface. Call the function through interface to notify the loading status if yes.
			if(PC->Implements<UBugLoadingScreenInterface>())
			{
				if (bIsVisible)
				{
					IBugLoadingScreenInterface::Execute_OnLoadingScreenActivated(PC);
				}
				else
				{
					IBugLoadingScreenInterface::Execute_OnLoadingScreenDeactivated(PC);
				}
			}
			
			if(APawn* OwningPawn = PC->GetPawn())
			{
				if (OwningPawn->Implements<UBugLoadingScreenInterface>())
				{
					if (bIsVisible)
					{
						IBugLoadingScreenInterface::Execute_OnLoadingScreenActivated(OwningPawn);
					}
					else
					{
						IBugLoadingScreenInterface::Execute_OnLoadingScreenDeactivated(OwningPawn);
					}
				}
			}
		}
	}
	
	//The Code for notifying other objects in the world goes here
	
}
