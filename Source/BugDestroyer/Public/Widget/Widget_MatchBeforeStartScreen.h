// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "Widget_ActivatableBase.h"
#include "Widget_MatchBeforeStartScreen.generated.h"

class ACommonGameState;
/**
 * 
 */
UCLASS()
class BUGDESTROYER_API UWidget_MatchBeforeStartScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnActivated() override;
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void OnWarmTimeChangedCallback(int32 InNewMinutes, int32 InNewSeconds);
	UFUNCTION(BlueprintImplementableEvent, Category = "BugDestroyer|UI")
	void BP_OnWarmTimeUpdated(int32 InNewMinutes, int32 InNewSeconds);
	
	void SetupGameStateBindings(ACommonGameState* GS);
	void OnGameStateSet(AGameStateBase* GameStateBase);
	
};
