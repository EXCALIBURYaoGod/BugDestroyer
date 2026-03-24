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
	void BroadcastElimAnnouncement(AController* VictimController, class AController* AttackerController, AActor* Causer, bool bHeadShot);
	// end AGameMode Interface
	
	virtual void PlayerEliminated(class ABugCharacter* VictimCharacter, AController* VictimController, class AController* AttackerController, AActor* Causer, bool bHeadShot);
	virtual void RequestRespawn(ACharacter* VictimCharacter, AController* VictimController);
	virtual float CalculateActualDamage(AController* Attacker, AController* Victim, float BaseDamage);
	
protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void OnMatchStateSet() override;
	
	FTimerHandle MatchTimerHandle;
	void UpdateMatchTime();
	FTimerHandle WarmupTimerHandle;
	void UpdateWarmupTime();
	virtual void HandleCooldownMatchState();
	FTimerHandle CooldownTimerHandle;
	void UpdateCooldownTime();
	virtual void HandleMatchEnd();
	UFUNCTION(Exec)
	void SetMatchTime(int32 newTime);
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Match")
	float WarmupTime = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Match")
	int32 MatchTime = 300;
	UPROPERTY(EditDefaultsOnly, Category = "Match")
	int32 CooldownTime = 10;
	
	
};
