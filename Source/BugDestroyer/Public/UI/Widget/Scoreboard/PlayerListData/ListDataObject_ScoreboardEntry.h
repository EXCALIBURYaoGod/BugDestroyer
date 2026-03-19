// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ListDataObject_ScoreboardEntry.generated.h"

class ACommonGamePlayerState;
/**
 * 
 */
UCLASS()
class BUGDESTROYER_API UListDataObject_ScoreboardEntry : public UObject
{
	GENERATED_BODY()
	
public:
	// 弱指针安全地持有 PlayerState
	TWeakObjectPtr<ACommonGamePlayerState> TargetPlayerState;

	// 提供给 UI 直接绑定的便捷函数
	UFUNCTION(BlueprintPure, Category = "Scoreboard")
	FString GetPlayerName() const;
	UFUNCTION(BlueprintPure, Category = "Scoreboard")
	int32 GetKills() const;
	UFUNCTION(BlueprintPure, Category = "Scoreboard")
	int32 GetDeaths() const;
	UFUNCTION(BlueprintPure, Category = "Scoreboard")
	int32 GetPing() const;
	UFUNCTION(BlueprintPure, Category = "Scoreboard")
	bool IsLocalPlayer() const;
	
};
