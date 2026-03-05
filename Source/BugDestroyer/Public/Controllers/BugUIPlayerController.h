// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "BugUIPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BUGDESTROYER_API ABugUIPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	// Begin APlayerController Interface
	virtual void OnPossess(APawn* InPawn) override;
	virtual void AcknowledgePossession(APawn* P) override;
	// end APlayerController Interface
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Pawn", meta = (DisplayName = "OnAcknowledgePossession"))
	void K2_OnAcknowledgePossession(APawn* P);
	
	
	
};
