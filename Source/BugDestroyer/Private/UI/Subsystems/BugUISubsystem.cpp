// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Subsystems/BugUISubsystem.h"

#include "BugGameplayTags.h"
#include "BugUIFunctionLibrary.h"
#include "Engine/AssetManager.h"
#include "Widget/Widget_ActivatableBase.h"
#include "Widget/Widget_ConfirmScreen.h"
#include "Widget/Widget_PrimaryLayout.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

UBugUISubsystem* UBugUISubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;
	
	APlayerController* PC = nullptr;
	
	if (const AActor* Actor = Cast<AActor>(WorldContextObject))
	{
	    PC = Cast<APlayerController>(const_cast<AActor*>(Actor));
	    if (!PC)
	    {
    		PC = Actor->GetGameInstance()->GetFirstLocalPlayerController();
	    }
	}
	else if (const UUserWidget* Widget = Cast<UUserWidget>(WorldContextObject))
	{
	    PC = Widget->GetOwningPlayer();
	}
	else if (const UWorld* World = Cast<UWorld>(WorldContextObject))
	{
		PC = World->GetFirstPlayerController();
	}
	
	if (PC && PC->GetLocalPlayer())
	{
	    return PC->GetLocalPlayer()->GetSubsystem<UBugUISubsystem>();
	}
	return nullptr;
	
}

bool UBugUISubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	// check: 1、只在服务器创建 2、保证全局仅一个子类创建实例
	/*if (!CastChecked<UGameInstance>(Outer)->IsDedicatedServerInstance())
	{
		TArray<UClass*> FoundClasses;
		GetDerivedClasses(GetClass(), FoundClasses);
		return FoundClasses.IsEmpty();
	}
	return false;*/
	if (!Cast<ULocalPlayer>(Outer))
	{
		return false;
	}
	return Super::ShouldCreateSubsystem(Outer);
}

void UBugUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UBugUISubsystem::RegisterCreatedPrimaryLayoutWidget(UWidget_PrimaryLayout* InCreatedPrimaryLayout)
{
	check(InCreatedPrimaryLayout);
	CreatedPrimaryLayout = InCreatedPrimaryLayout;
}

void UBugUISubsystem::UnRegisterCreatedPrimaryLayoutWidget(UWidget_PrimaryLayout* InCreatedPrimaryLayout)
{
	if (CreatedPrimaryLayout == InCreatedPrimaryLayout)
	{
		
		CreatedPrimaryLayout = nullptr;
	}
}

void UBugUISubsystem::PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag,
                                                 TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,
                                                 TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)> AsyncPushStateCallBack)
{
	check(!InSoftWidgetClass.IsNull());
	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		InSoftWidgetClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[InSoftWidgetClass, this, InWidgetStackTag, AsyncPushStateCallBack]()
			{
				UClass* LoadedWidgetClass = InSoftWidgetClass.Get();
				UCommonActivatableWidgetContainerBase* FoundWidgetStack = CreatedPrimaryLayout->FindWidgetStackByTag(InWidgetStackTag);
				UWidget_ActivatableBase* CreatedWidget = FoundWidgetStack->AddWidget<UWidget_ActivatableBase>(
					LoadedWidgetClass,
					[AsyncPushStateCallBack](UWidget_ActivatableBase& CreatedWidgetInstance)
					{
						AsyncPushStateCallBack(EAsyncPushWidgetState::OnCreatedBeforePush, &CreatedWidgetInstance);
					}
				);
				AsyncPushStateCallBack(EAsyncPushWidgetState::AfterPush, CreatedWidget);
			}
		)
	);
}

void UBugUISubsystem::PushConfirmScreenToModalStackAsync(EConfirmScreenType InScreenType, const FText& InScreenTitle,
	const FText& InScreenMsg, TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallback)
{
	UConfirmScreenInfoObject* CreatedInfoObject = nullptr;
	
	switch (InScreenType)
	{
		case EConfirmScreenType::Ok:
			CreatedInfoObject = UConfirmScreenInfoObject::CreateOkScreen(InScreenTitle, InScreenMsg);
			
			break;
		
		case EConfirmScreenType::YesNo:
			CreatedInfoObject = UConfirmScreenInfoObject::CreateYesNoScreen(InScreenTitle, InScreenMsg);
			break;
		
		case EConfirmScreenType::OkCancel:
			CreatedInfoObject = UConfirmScreenInfoObject::CreateOkCancelScreen(InScreenTitle, InScreenMsg);
			break;
		
		case EConfirmScreenType::Unknown:
			break;
		default:
			break;
	}
	
	check(CreatedInfoObject);
	
	PushSoftWidgetToStackAsync(
		BugGameplayTags::Bug_WidgetStack_Modal,
		UBugUIFunctionLibrary::GetSoftWidgetClassByTag(BugGameplayTags::Bug_Widget_ConfirmScreen),
		[CreatedInfoObject, ButtonClickedCallback](EAsyncPushWidgetState InPushWidgetState, UWidget_ActivatableBase* InPushWidget)
		{
			if (InPushWidgetState == EAsyncPushWidgetState::OnCreatedBeforePush)
			{
				UWidget_ConfirmScreen* CreatedConfirmScreen = CastChecked<UWidget_ConfirmScreen>(InPushWidget);
				CreatedConfirmScreen->InitConfirmScreen(CreatedInfoObject, ButtonClickedCallback);
			}
		}
	);
}

void UBugUISubsystem::NotifyPawnResubscribed(APawn* NewPawn)
{
	if (OnPawnResubscribedDelegate.IsBound())
	{
		OnPawnResubscribedDelegate.Broadcast(NewPawn);
	}
}

UWidget_ActivatableBase* UBugUISubsystem::PushWidgetToStack(const FGameplayTag& InWidgetStackTag, UClass* InWidgetClass)
{
	if (!InWidgetClass || !CreatedPrimaryLayout) return nullptr;
	UCommonActivatableWidgetContainerBase* FoundWidgetStack = CreatedPrimaryLayout->FindWidgetStackByTag(InWidgetStackTag);
	if (FoundWidgetStack)
	{
		return FoundWidgetStack->AddWidget<UWidget_ActivatableBase>(InWidgetClass);
	}
	return nullptr;
}

UWidget_ActivatableBase* UBugUISubsystem::PushSoftWidgetToStack(const FGameplayTag& InWidgetStackTag,
	TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass)
{
	if (InSoftWidgetClass.IsNull()) return nullptr;
	UClass* LoadedClass = InSoftWidgetClass.LoadSynchronous();
	return PushWidgetToStack(InWidgetStackTag, LoadedClass);
}

UWidget_PrimaryLayout* UBugUISubsystem::GetCreatedPrimaryLayout() const
{

	if (IsValid(CreatedPrimaryLayout))
	{
		return CreatedPrimaryLayout;
	}
	return nullptr;
}

