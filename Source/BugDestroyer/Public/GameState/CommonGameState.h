// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "CommonGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMatchTimeUpdated, int32, Minutes, int32, Seconds);

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
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMatchTimeUpdated OnMatchTimeUpdated;
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_MatchTime, BlueprintReadOnly, VisibleAnywhere, Category = "Match", meta = (AllowPrivateAccess = true))
	int32 MatchTime = 300;
	UFUNCTION()
	void OnRep_MatchTime();
	
public:
	void GetFormattedTime(int32& Minutes, int32& Seconds) const;
	FORCEINLINE void SetMatchTime(int32 NewMatchTime) { if (HasAuthority()) MatchTime = NewMatchTime; }
	FORCEINLINE int32 GetMatchTime() const { return MatchTime; }
	
};
