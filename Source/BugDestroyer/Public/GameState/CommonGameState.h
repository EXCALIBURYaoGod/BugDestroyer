// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CommonGameState.generated.h"

class ACommonGamePlayerState;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMatchTimeUpdated, int32, Minutes, int32, Seconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWarmupTimeUpdated, int32, Minutes, int32, Seconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCooldownTimeUpdated, int32, Minutes, int32, Seconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMVPNameUpdated, const FText&, WinnerNames);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWinnerTeamUpdated, const FText&, WinnerTeam);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnKillMessageBroadcast, const FString&, KillerName, const FString&, VictimName, const FString&, WeaponName, bool, bIsHeadshot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTeamScoreUpdated, int32, RedTeamScore, int32, BlueTeamScore);


/**
 * 通用比赛状态类，负责存储全局比赛数据
 */
UCLASS()
class BUGDESTROYER_API ACommonGameState : public AGameState
{
	GENERATED_BODY()
public:
	ACommonGameState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_BroadcastKillMessage(const FString& KillerName, const FString& VictimName, const FString& WeaponName, bool bIsHeadshot);
	
	UPROPERTY(BlueprintAssignable, Category = "Events|Match")
	FOnKillMessageBroadcast OnKillMessageBroadcast;
	UPROPERTY(BlueprintAssignable, Category = "Events|Match")
	FOnMatchTimeUpdated OnMatchTimeUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Events|Match")
	FOnWarmupTimeUpdated OnWarmupTimeUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Events|Match")
	FOnCooldownTimeUpdated OnCooldownTimeUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Events|Match")
	FOnMVPNameUpdated OnMVPNameUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Events|Match")
	FOnTeamScoreUpdated OnTeamScoreUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Events|Match")
	FOnWinnerTeamUpdated OnWinnerTeamUpdated;
	
	// == Teams == //
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Teams|Match")
	int32 WinningScore = 100;
	UPROPERTY(ReplicatedUsing= OnRep_WinnerTeam, VisibleAnywhere, Category = "Teams|Match")
	FText WinnerTeam;
	UPROPERTY(VisibleAnywhere, Category = "Teams|Match")
	TArray<ACommonGamePlayerState*> RedTeam;
	UPROPERTY(VisibleAnywhere, Category = "Teams|Match")
	TArray<ACommonGamePlayerState*> BlueTeam;
	UPROPERTY(ReplicatedUsing=OnRep_RedTeamScore)
	int32 RedTeamScore;
	UPROPERTY(ReplicatedUsing=OnRep_BlueTeamScore)
	int32 BlueTeamScore;
	void AddToRedTeamScore();
	void AddToBlueTeamScore();
	UFUNCTION()
	void OnRep_RedTeamScore();
	UFUNCTION()
	void OnRep_BlueTeamScore();
	UFUNCTION()
	void OnRep_WinnerTeam();
	// == Teams == //
	
protected:
	virtual void BeginPlay() override;
	virtual void OnRep_MatchState() override;
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_MatchTime, BlueprintReadOnly, VisibleAnywhere, Category = "Match", meta = (AllowPrivateAccess = true))
	int32 MatchTime = 300;
	UFUNCTION()
	void OnRep_MatchTime();
	UPROPERTY(ReplicatedUsing = OnRep_WarmupTime, BlueprintReadOnly, VisibleAnywhere, Category = "Match", meta = (AllowPrivateAccess = true))
	int32 WarmupTime = 10;
	UFUNCTION()
	void OnRep_WarmupTime();
	UPROPERTY(ReplicatedUsing = OnRep_CooldownTime, BlueprintReadOnly, VisibleAnywhere, Category = "Match", meta = (AllowPrivateAccess = true))
	int32 CooldownTime = 10;
	UFUNCTION()
	void OnRep_CooldownTime();
	
public:
	FORCEINLINE void SetMatchTime(int32 NewMatchTime) { if (HasAuthority()) MatchTime = NewMatchTime; }
	FORCEINLINE int32 GetMatchTime() const { return MatchTime; }
	FORCEINLINE void SetWarmupTime(int32 NewWarmupTime) { if (HasAuthority()) WarmupTime = NewWarmupTime; }
	FORCEINLINE int32 GetWarmupTime() const { return WarmupTime; }
	FORCEINLINE void SetCooldownTime(int32 NewCooldownTime) { if (HasAuthority()) CooldownTime = NewCooldownTime; }
	FORCEINLINE int32 GetCooldownTime() const { return CooldownTime; }
	
};
