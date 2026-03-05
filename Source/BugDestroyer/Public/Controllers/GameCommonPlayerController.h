// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameCommonPlayerController.generated.h"

class UWidget_PrimaryLayout;
/**
 * 
 */
UCLASS()
class BUGDESTROYER_API AGameCommonPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	// Listen Server 主机 (Server0) 附身 Pawn 时触发
	virtual void OnPossess(APawn* aPawn) override;
	// 客户端 (Client1) 确认附身 Pawn 时触发
	virtual void AcknowledgePossession(APawn* aPawn) override;
	
	// 提取出一个共用的初始化函数
	void InitUIWithCharacter(APawn* InPawn);
	void CreateUI(APawn* InPawn);
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UWidget_PrimaryLayout> GamePrimaryLayout;
	
};
