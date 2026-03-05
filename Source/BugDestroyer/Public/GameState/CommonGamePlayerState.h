// //Copyrights @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CommonGamePlayerState.generated.h"

class AGameCommonPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, float, NewScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDefeatsChanged, int32, NewDefeats);

/**
 * 通用PlayerState, 默认
 */
UCLASS()
class BUGDESTROYER_API ACommonGamePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	// begin AActor Interface
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Reset() override;
	// end AActor Interface
	
	// begin APlayerState interface
	virtual void OnRep_Score() override;
	// end APlayerState interface
	UFUNCTION()
	void OnRep_Defeats();
	
	void AddToScore(float InScore);
	void AddToDefeats(int32 InDefeats);
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnScoreChanged OnScoreChanged;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDefeatsChanged OnDefeatsChanged;
	
protected:
	// begin AActor interface
	virtual void PostInitializeComponents() override;
	// end AActor interface
	
	UFUNCTION()
	void HandleOnPawnSet(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);
	
private:
	UPROPERTY()
	AGameCommonPlayerController* GameCommonPC;
	
	UPROPERTY(ReplicatedUsing=OnRep_Defeats)
	int32 Defeats = 0;
	
	
public:
	FORCEINLINE int32 GetDefeats() const { return Defeats; }
	FORCEINLINE void SetDefeats(const int32 InDefeatsAmount) { Defeats = InDefeatsAmount; }
	
};
