// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget_ListEntry_Base.h"

#include "Widget_ListEntry_SessionEntry.generated.h"

class UMultiplayerSessionsSubsystem;
class UListDataObject_Session;
class USessionEntryData;
class UBugCommonButtonBase;

/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class BUGDESTROYER_API UWidget_ListEntry_SessionEntry : public UWidget_ListEntry_Base
{
	GENERATED_BODY()
	
protected:
	//begin UWidget_ListEntry_Base Interface
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject) override;
	//end UWidget_ListEntry_Base Interface
	
	UFUNCTION(BlueprintCallable)
	void JoinGameByIndex();
	
private:
	// 缓存转换为具体类型的数据指针
	UPROPERTY(Transient)
	UListDataObject_Session* CachedSessionData;
	UPROPERTY()
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;
	
	
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonTextBlock* CommonText_ServerName;
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonTextBlock* CommonText_Index;
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonTextBlock* CommonText_Ping;
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonTextBlock* CommonText_CurrentMaxPlayers;
	
	
};
