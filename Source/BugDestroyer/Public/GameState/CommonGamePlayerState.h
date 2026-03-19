// //Copyrights @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "BugTypes/BugEnumTypes.h"
#include "GameFramework/PlayerState.h"
#include "CommonGamePlayerState.generated.h"

class AGameCommonPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDefeatsChanged, int32, NewDefeats);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKillsChanged, int32, NewKills);

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
	
	UFUNCTION()
	void OnRep_Defeats();
	UFUNCTION()
	void OnRep_Kills();
	
	void AddToDefeats(int32 InDefeats);
	void AddToKills(int32 InKills);
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDefeatsChanged OnDefeatsChanged;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDefeatsChanged OnKillsChanged;
	
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
	UPROPERTY(ReplicatedUsing=OnRep_Kills)
	int32 Kills = 0;
	UPROPERTY(Replicated)
	ETeam Team = ETeam::ET_NoneTeam;
	
public:
	FORCEINLINE int32 GetDefeats() const { return Defeats; }
	FORCEINLINE void SetDefeats(const int32 InDefeatsAmount) { Defeats = InDefeatsAmount; }
	FORCEINLINE int32 GetKills() const { return Kills; }
	FORCEINLINE void SetKills(const int32 InKills) { Kills = InKills; }
	FORCEINLINE ETeam GetTeam() const { return Team; }
	FORCEINLINE void SetTeam(const ETeam NewTeam) { Team = NewTeam; }
	
};
