// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "Widget_ActivatableBase.h"
#include "GameState/CommonGameState.h"
#include "Widget_CharacterScreen.generated.h"

class ABugCharacter;
/**
 * 玩家界面，负责显示血量、弹药、小地图等玩家组件
 */
UCLASS()
class BUGDESTROYER_API UWidget_CharacterScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()
	
public:
	
protected:
	// begin UCommonActivatableWidget Interface
	virtual void NativeOnActivated() override;
	// end UCommonActivatableWidget Interface

	// begin UUserWidget Interface
	virtual void NativeConstruct() override;
	// end UUserWidget Interface
	
	UFUNCTION()
	void OnHandlePawnResubscribed(APawn* NewPawn);
	UFUNCTION(BlueprintImplementableEvent, Category = "BugDestroyer|UI")
	void BP_OnHealthUpdated(float NewHealth, float MaxHealth);
	UFUNCTION()
	void OnHealthChangedCallback(float NewHealth, float MaxHealth);
	UFUNCTION(BlueprintImplementableEvent, Category = "BugDestroyer|UI")
	void BP_OnShieldUpdated(float NewShield, float MaxShield);
	UFUNCTION()
	void OnShieldChangedCallback(float NewShield, float MaxShield);
	UFUNCTION(BlueprintImplementableEvent, Category = "BugDestroyer|UI")
	void BP_OnBrokenShield();
	UFUNCTION()
	void OnBrokenShieldCallback();
	UFUNCTION()
	void OnAmmoChangedCallback(int32 InCurrentAmmo, int32 InMagCapacity, int32 InAmmoLeft);
	UFUNCTION(BlueprintImplementableEvent, Category = "BugDestroyer|UI")
	void BP_OnAmmoUpdated(int32 InCurrentAmmo, int32 InMagCapacity, int32 InAmmoLeft);
	UFUNCTION()
	void OnWeaponChangedCallback(class AWeapon* NewWeapon);
	UFUNCTION(BlueprintImplementableEvent, Category = "BugDestroyer|UI")
	void BP_SetAmmoHUDVisibility(bool bIsVisible);
	UFUNCTION()
	void OnAmmoLeftChangedCallback(int32 InCurrentAmmo, int32 InMagCapacity, int32 InAmmoLeft);
	UFUNCTION(BlueprintImplementableEvent, Category = "BugDestroyer|UI")
	void BP_OnAmmoLeftUpdated(int32 InCurrentAmmo, int32 InMagCapacity, int32 InAmmoLeft);
	UFUNCTION()
	void OnMatchTimeChangedCallback(int32 InNewMinutes, int32 InNewSeconds);
	UFUNCTION(BlueprintImplementableEvent, Category = "BugDestroyer|UI")
	void BP_OnMatchTimeUpdated(int32 InNewMinutes, int32 InNewSeconds);
	UFUNCTION()
	void OnMatchScoreChangedCallback(int32 InNewScore);
	UFUNCTION(BlueprintImplementableEvent, Category = "BugDestroyer|UI")
	void BP_OnMatchScoreUpdated(int32 InNewScore);
	UFUNCTION()
	void OnTeamScoreChangedCallback(int32 InRedTeamScore, int32 InBlueTeamScore);
	UPROPERTY()
	ACommonGamePlayerState* CachedPlayerState;
	UFUNCTION(BlueprintImplementableEvent, Category = "BugDestroyer|UI")
	void BP_OnTeamScoreUpdated(int32 InOurTeamScore, int32 InEnemyTeamScore);
	UFUNCTION()
	void OnGrenadeAmountChangedCallback(int32 InCurrentGrenadeAmount);
	UFUNCTION(BlueprintImplementableEvent, Category = "BugDestroyer|UI")
	void BP_OnGrenadeAmountUpdated(int32 InCurrentGrenadeAmount);
	UFUNCTION(BlueprintImplementableEvent, Category = "BugDestroyer|UI")
	void BP_SetGrenadeHUDVisibility(bool bIsVisible);
	UFUNCTION()
	void OnNetWarningCallback(bool bIsVisible);
	UFUNCTION(BlueprintImplementableEvent, Category = "BugDestroyer|UI")
	void BP_SetNetWarningHUDVisibility(bool bIsVisible);
	void OnGameStateSet(AGameStateBase* GameStateBase);
	void SetupGameStateBindings(ACommonGameState* GS);
	UFUNCTION()
	void OnKillMessageReceivedCallback(const FString& KillerName, const FString& VictimName, const FString& AttackWeaponName, bool bIsHeadshot);
	UFUNCTION(BlueprintImplementableEvent, Category = "BugDestroyer|UI")
	void BP_OnKillMessageReceived(const FString& KillerName, const FString& VictimName, const TSoftObjectPtr<UTexture2D>& WeaponIcon, bool bIsHeadshot, bool bGeneric);
	
	UFUNCTION(BlueprintPure)
	ABugCharacter* GetOwningCharacter();
	UPROPERTY(BlueprintReadOnly)
	AWeapon* CachedEquippedWeapon;
	UPROPERTY(EditDefaultsOnly, Category = "BugDestroyer|Data")
	UDataTable* WeaponUIDataTable;
	
private:

	
};
