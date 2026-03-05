// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BugTypes/BugEnumTypes.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncAction_PushConfirmScreen.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConfirmScreenButtonClickedDelegate, EConfirmScreenButtonType, InClickedButtonType);

/**
 * 
 */
UCLASS()
class BUGDESTROYER_API UAsyncAction_PushConfirmScreen : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", 
		HidePin = "WorldContextObject", BlueprintInternalUseOnly = "true", DisplayName = "Show Confirmation Screen"))
	static UAsyncAction_PushConfirmScreen* PushConfirmScreen(
		const UObject* WorldContextObject, 
		APlayerController* OwningPlayerController,
		EConfirmScreenType ConfirmScreenType,
		FText InScreenTitle,
		FText InScreenMsg
	);
	
	// Begin UBlueprintAsyncActionBase Interface
	virtual void Activate() override;
	// End UBlueprintAsyncActionBase Interface
	
	UPROPERTY(BlueprintAssignable)
	FOnConfirmScreenButtonClickedDelegate OnConfirmScreenButtonClicked;
	
private:
	
	TWeakObjectPtr<UWorld> CachedOwningWorld;
	TWeakObjectPtr<APlayerController> CachedOwningPC;
	EConfirmScreenType CachedConfirmScreenType;
	FText CachedScreenTitle;
	FText CachedScreenMsg;
	
};

