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
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;
	void InitPlayerTeam(AController* NewController);
	virtual void InitSeamlessTravelPlayer(AController* NewController) override;
	virtual void HandleMatchIsWaitingToStart() override;
	// end AGamemode Interface
	
	// begin ACommonGameMode Interface
	virtual void HandleMatchEnd() override;
	virtual float CalculateActualDamage(AController* Attacker, AController* Victim, float BaseDamage) override;
	virtual void PlayerEliminated(class ABugCharacter* VictimCharacter, AController* VictimController, class AController* AttackerController, AActor* Causer, bool bHeadShot) override;
	virtual void HandleCooldownMatchState() override;
	// end ACommonGameMode Interface
	
protected:
	// begin AGamemode Interface
	virtual void HandleMatchHasStarted() override;
	//end AGamemode Interface
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="TeamMatch")
	int32 MatchScore = 100;
	
};
