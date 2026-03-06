// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameCommonPlayerController.generated.h"

class UWidget_MatchCooldownScreen;
class UWidget_ActivatableBase;
class UWidget_PrimaryLayout;
/**
 * 
 */
UCLASS()
class BUGDESTROYER_API AGameCommonPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	UFUNCTION(Client, Reliable)
	void ClientSetMatchState(FName NewState);
	UFUNCTION(Client, Reliable)
	void Client_ShowMatchCooldown(const FText& WinnerNames);
	
protected:

	virtual void OnPossess(APawn* aPawn) override;
	virtual void AcknowledgePossession(APawn* aPawn) override;
	virtual void ReceivedPlayer() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	void InitUIWithCharacter(APawn* InPawn);
	void CreatePrimaryLayout();
	void PushMatchBeforeStartScreen();
	void PushCharacterScreen(APawn* InPawn);
	void PushMatchCooldownScreen(TFunction<void(UWidget_MatchCooldownScreen*)> OnCreatedCallback);
	
	void HandleMatchCooldownState();
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UWidget_PrimaryLayout> GamePrimaryLayout;
	UPROPERTY(VisibleAnywhere)
	FName MatchState;
	bool bCharacterScreenPushed = false;

	
};
