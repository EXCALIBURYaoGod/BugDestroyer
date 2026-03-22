// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonGameMode.h"
#include "TeamsGameMode.generated.h"

/**
 * 团队竞技模式
 */
UCLASS()
class BUGDESTROYER_API ATeamsGameMode : public ACommonGameMode
{
	GENERATED_BODY()
	
public:
	// begin AGamemode Interface
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	// end AGamemode Interface
	
	// begin ACommonGameMode Interface
	virtual void HandleMatchEnd() override;
	virtual float CalculateActualDamage(AController* Attacker, AController* Victim, float BaseDamage) override;
	virtual void PlayerEliminated(class ABugCharacter* VictimCharacter, AController* VictimController, class AController* AttackerController, AActor* Causer, bool bHeadShot) override;
	// end ACommonGameMode Interface
	
protected:
	// begin AGamemode Interface
	virtual void HandleMatchHasStarted() override;
	//end AGamemode Interface
	
	
};
