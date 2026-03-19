// Copyrights @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameCommonPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
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
	AGameCommonPlayerController();
	
	UFUNCTION(Client, Reliable)
	void ClientSetMatchState(FName NewState);
	UFUNCTION(Client, Reliable)
	void Client_ShowMatchCooldown(const FText& WinnerNames);
	void ShowSniperScopeWidget(bool bIsShow);
	
protected:
	// Begin APlayerController Interface
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void AcknowledgePossession(APawn* aPawn) override;
	virtual void ReceivedPlayer() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// End APlayerController Interface
	
	void InitUIWithCharacter(APawn* InPawn);
	void CreatePrimaryLayout();
	void PushMatchBeforeStartScreen();
	void PushCharacterScreen(APawn* InPawn);
	void PushMatchCooldownScreen(TFunction<void(UWidget_MatchCooldownScreen*)> OnCreatedCallback);
	void HandleMatchCooldownState();
	
	// == NetWork Check == //
	void UpdateNetworkStats();
	UFUNCTION(Server, Reliable)
	void ServerSetWeaponSSR(bool bEnableSSR);
	FTimerHandle NetStatTimerHandle;
	UPROPERTY(BlueprintReadOnly, Category = "Network", meta = (AllowPrivateAccess = true))
	float CurrentPing;
	UPROPERTY(BlueprintReadOnly, Category = "Network", meta = (AllowPrivateAccess = true))
	float PacketLossPercentage;
	bool bLastNetWarning = false;
	// == NetWork Check == //
	
	// == Input == //
	virtual void SetupInputComponent() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleScoreboardAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleMenuAction;
	void ToggleGameMenu();
	void ToggleScoreboardButtonPressed();
	void ToggleScoreboardButtonReleased();
	// == Input == //
	
private:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UWidget_PrimaryLayout> GamePrimaryLayout;
	UPROPERTY(VisibleAnywhere, Category = "Match")
	FName MatchState;
	bool bCharacterScreenPushed = false;
	bool bScoreboardButtonPressed = false;
	bool bSniperScopeButtonPressed = false;
	UPROPERTY()
	UWidget_ActivatableBase* CachedSniperScopeWidget;
	UPROPERTY(Transient)
	UWidget_ActivatableBase* CachedScoreboardWidget;
	
	
	// == 客户端与服务端的时钟同步 == //
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float ClientRequestTime);
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float ClientRequestTime, float TimeServerReceived);
	void CheckTimeSync();
	float ClientServerDelta;
	int32 SyncCount;
	FTimerHandle TimeSyncTimer;
	// == 客户端与服务端的时钟同步 == //
	
public:
	FORCEINLINE float GetClientServerDelta() const { return ClientServerDelta; }
	
};
