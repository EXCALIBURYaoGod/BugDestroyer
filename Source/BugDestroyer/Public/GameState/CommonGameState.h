// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CommonGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMatchTimeUpdated, int32, Minutes, int32, Seconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWarmupTimeUpdated, int32, Minutes, int32, Seconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCooldownTimeUpdated, int32, Minutes, int32, Seconds);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWinnerNameUpdated, const FText&, WinnerNames);

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
	
	UPROPERTY(BlueprintAssignable, Category = "Events|Match")
	FOnMatchTimeUpdated OnMatchTimeUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Events|Match")
	FOnWarmupTimeUpdated OnWarmupTimeUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Events|Match")
	FOnCooldownTimeUpdated OnCooldownTimeUpdated;
	UPROPERTY(BlueprintAssignable, Category = "Events|Match")
	FOnWinnerNameUpdated OnWinnerNameUpdated;
	
protected:
	virtual void BeginPlay() override;
	
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
