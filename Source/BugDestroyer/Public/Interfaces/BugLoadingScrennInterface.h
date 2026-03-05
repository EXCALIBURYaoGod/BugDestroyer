
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BugLoadingScrennInterface.generated.h"

UINTERFACE(BlueprintType, MinimalAPI)
class UBugLoadingScreenInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BUGDESTROYER_API IBugLoadingScreenInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void OnLoadingScreenActivated();
	/* virtual void OnLoadingScreenActivated_Implementation(); */
	
	UFUNCTION(BlueprintNativeEvent)
	void OnLoadingScreenDeactivated();
	/* virtual void OnLoadingScreenDeactivated_Implementation(); */
	
};
