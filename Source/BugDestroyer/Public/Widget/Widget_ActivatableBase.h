// Copyrights @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "Widget_ActivatableBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta = (DisableNaiveTick))
class BUGDESTROYER_API UWidget_ActivatableBase : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(BlueprintPure)
	APlayerController* GetOwningPlayerController();
	
private:
	TWeakObjectPtr<APlayerController> CachedOwningPlayerController;
	
};
