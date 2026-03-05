// Copyrights @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CommonGameMode.generated.h"

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
	FTimerHandle MatchTimerHandle;
	void UpdateMatchTime();
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Match")
	float WarmupTime = 10.f;
	float CountdownTime = 0.f;
	
};
