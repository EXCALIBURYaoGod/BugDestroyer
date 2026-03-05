// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget_ActivatableBase.h"
#include "Data/SessionInfo.h"

#include "Widget_MultiplayerScreen.generated.h"


class UBugCommonListView;
class UMultiplayerSessionsSubsystem;

/**
 * 多人游戏UI界面
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class BUGDESTROYER_API UWidget_MultiplayerScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable)
	void HostGame(int32 NumPublicConnections, FString MatchType);
	UFUNCTION(BlueprintCallable)
	void SearchGame();
	
protected:
	// begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	// end UUserWidget Interface
	
private:
	UPROPERTY()
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;
	
	UFUNCTION()
	void OnCreateSessionComplete(bool bWasSuccessful);
	UFUNCTION()
	void OnFindSessionsComplete(const TArray<FSessionInfo>& SessionResults, bool bWasSuccessful);
	UFUNCTION()
	void OnJoinSessionComplete(bool bWasSuccessful, FString Address);
	
	// bind widgets //
	UPROPERTY(meta = (BindWidget))
	UBugCommonListView* CommonListView_SessionListView;
	// bind widgets //
	
};
