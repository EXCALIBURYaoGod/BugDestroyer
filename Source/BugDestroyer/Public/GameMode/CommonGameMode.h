// Copyrights @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CommonGameMode.generated.h"

namespace MatchState
{
	extern BUGDESTROYER_API const FName Cooldown;
}

UCLASS()
class BUGDESTROYER_API ACommonGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ACommonGameMode();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	// begin AGameMode Interface
	virtual void HandleMatchHasStarted() override;
	// end AGameMode Interface
	
	virtual void PlayerEliminated(class ABugCharacter* VictimCharacter, AController* VictimController, class AController* AttackerController);
	virtual void RequestRespawn(ACharacter* VictimCharacter, AController* VictimController);
	
	
protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void OnMatchStateSet() override;
	
	FTimerHandle MatchTimerHandle;
	void UpdateMatchTime();
	FTimerHandle WarmupTimerHandle;
	void UpdateWarmupTime();
	void HandleCooldownMatchState();
	FTimerHandle CooldownTimerHandle;
	void UpdateCooldownTime();
	void HandleMatchEnd();
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Match")
	float WarmupTime = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Match")
	int32 MatchTime = 300;
	UPROPERTY(EditDefaultsOnly, Category = "Match")
	int32 CooldownTime = 10;
	
	
};
