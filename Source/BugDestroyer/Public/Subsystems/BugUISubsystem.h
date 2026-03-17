// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BugTypes/BugEnumTypes.h"
#include "Engine/GameInstance.h"
#include "Widget/Widget_ActivatableBase.h"
#include "BugUISubsystem.generated.h"

class UBugCommonButtonBase;
class UWidget_ActivatableBase;
struct FGameplayTag;
class UWidget_PrimaryLayout;

enum class EAsyncPushWidgetState : uint8
{
	OnCreatedBeforePush,
	AfterPush
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnButtonDescriptionTextUpdatedDelegate, 
	UBugCommonButtonBase*, BroadcastingButton, FText, InText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnResubscribedDelegate, APawn*, NewPawn);

/**
 * 
 */
UCLASS()
class BUGDESTROYER_API UBugUISubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	static UBugUISubsystem* Get(const UObject* WorldContextObject);
	
	// begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	// end USubsystem Interface
	
	UFUNCTION(BlueprintCallable)
	void RegisterCreatedPrimaryLayoutWidget(UWidget_PrimaryLayout* InCreatedPrimaryLayout);
	UFUNCTION(BlueprintCallable)
	void UnRegisterCreatedPrimaryLayoutWidget(UWidget_PrimaryLayout* InCreatedPrimaryLayout);
	
	void PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag, 
		TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass,
		TFunction<void(EAsyncPushWidgetState, UWidget_ActivatableBase*)> AsyncPushStateCallBack = [](EAsyncPushWidgetState, UWidget_ActivatableBase*){}
	);
	
	void PushConfirmScreenToModalStackAsync(EConfirmScreenType InScreenType, const FText& InScreenTitle, const FText& InScreenMsg,
		TFunction<void(EConfirmScreenButtonType)> ButtonClickedCallback);
	
	UPROPERTY(BlueprintAssignable)
	FOnButtonDescriptionTextUpdatedDelegate OnButtonDescriptionTextUpdatedDelegate;
	UPROPERTY(BlueprintAssignable, Category = "BugDestroyer|UI")
	FOnPawnResubscribedDelegate OnPawnResubscribedDelegate;
	/** 当新 Pawn 重生并被控制时调用 */
	UFUNCTION(BlueprintCallable, Category = "BugDestroyer|UI")
	void NotifyPawnResubscribed(APawn* NewPawn);
	
	/** 同步推入 Widget 到指定的栈 */
	UWidget_ActivatableBase* PushWidgetToStack(const FGameplayTag& InWidgetStackTag, UClass* InWidgetClass);

	/** 同步加载并推入 Widget（会产生微量阻塞） */
	UWidget_ActivatableBase* PushSoftWidgetToStack(const FGameplayTag& InWidgetStackTag, TSoftClassPtr<UWidget_ActivatableBase> InSoftWidgetClass);
	
private:
	UPROPERTY(Transient)
	UWidget_PrimaryLayout* CreatedPrimaryLayout;
	
public:
	UWidget_PrimaryLayout* GetCreatedPrimaryLayout() const;
	
	
};
