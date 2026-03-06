// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "Widget_ActivatableBase.h"
#include "Widget_MatchCooldownScreen.generated.h"

class ACommonGameState;
/**
 * 
 */
UCLASS()
class BUGDESTROYER_API UWidget_MatchCooldownScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()
public:
	void OnWinnerNameChangedCallback(const FText& WinnerNames);
	
protected:
	virtual void NativeOnActivated() override;
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnCooldownTimeChangedCallback(int32 InNewMinutes, int32 InNewSeconds);
	UFUNCTION(BlueprintImplementableEvent, Category = "BugDestroyer|UI")
	void BP_OnCooldownTimeUpdated(int32 InNewMinutes, int32 InNewSeconds);
	UFUNCTION(BlueprintImplementableEvent, Category = "BugDestroyer|UI")
	void BP_OnWinnerNameUpdated(const FText& WinnerNames);
	
	void SetupGameStateBindings(ACommonGameState* GS);
	void OnGameStateSet(AGameStateBase* GameStateBase);
};
