// Copyrights @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameCommonPlayerController.generated.h"

class UWidget_MatchCooldownScreen;
class UWidget_ActivatableBase;
class UWidget_PrimaryLayout;

/**
 * Game common player controller with UI management and match state functionality
 */
UCLASS()
class BUGDESTROYER_API AGameCommonPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/** Constructor */
	AGameCommonPlayerController();
	
	/** RPC to set the match state on the client */
	UFUNCTION(Client, Reliable)
	void ClientSetMatchState(FName NewState);
	
	/** RPC to show match cooldown screen on the client */
	UFUNCTION(Client, Reliable)
	void Client_ShowMatchCooldown(const FText& WinnerNames);
	
	/** Show or hide the sniper scope widget */
	void ShowSniperScopeWidget(bool bIsShow);
	
protected:
	// Begin APlayerController Interface
	virtual void OnPossess(APawn* aPawn) override;
	virtual void AcknowledgePossession(APawn* aPawn) override;
	virtual void ReceivedPlayer() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// End APlayerController Interface
	
	/** Initialize UI when possessing a character */
	void InitUIWithCharacter(APawn* InPawn);
	
	/** Create the primary UI layout */
	void CreatePrimaryLayout();
	
	/** Push the match before start screen */
	void PushMatchBeforeStartScreen();
	
	/** Push the character screen */
	void PushCharacterScreen(APawn* InPawn);
	
	/** Push the match cooldown screen with callback */
	void PushMatchCooldownScreen(TFunction<void(UWidget_MatchCooldownScreen*)> OnCreatedCallback);
	
	/** Handle match cooldown state changes */
	void HandleMatchCooldownState();
	
private:
	/** Primary layout widget class */
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UWidget_PrimaryLayout> GamePrimaryLayout;
	
	/** Current match state */
	UPROPERTY(VisibleAnywhere, Category = "Match")
	FName MatchState;
	
	/** Whether the character screen has been pushed */
	bool bCharacterScreenPushed = false;
	
	/** Cached reference to the sniper scope widget */
	UPROPERTY()
	UWidget_ActivatableBase* CachedSniperScopeWidget;
};
