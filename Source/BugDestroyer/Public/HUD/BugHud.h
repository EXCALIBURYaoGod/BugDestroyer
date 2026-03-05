// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "DebugHelper.h"
#include "BugTypes/BugStructTypes.h"
#include "GameFramework/HUD.h"
#include "BugHud.generated.h"

class UWidget_Crosshair;
/**
 * 玩家HUD
 */
UCLASS()
class BUGDESTROYER_API ABugHud : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaTime) override;
	void DrawCrosshair();
	// begin AHUD Interface
	virtual void DrawHUD() override;
	// end AHUD Interface
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair", meta = (AllowPrivateAccess = true))
	FCrosshairSettings Config;
	FCrosshairSettings DefaultConfig;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair", meta = (AllowPrivateAccess = true))
	float CurrentSpread = 0.0f;
	bool bDrawCrosshair = false;
	
public:
	FORCEINLINE void SetbDrawCrosshair(bool InbDrawCrosshair) { bDrawCrosshair = InbDrawCrosshair;}
	FORCEINLINE void SetCrosshairCurrentSpread(float InCurrentSpread) { CurrentSpread = InCurrentSpread; }
	FORCEINLINE float GetCrosshairCurrentSpread() const { return CurrentSpread; }
	FORCEINLINE void SetCrosshairGap(float InGap) { Config.Gap = InGap; }
	FORCEINLINE float GetCrosshairGap() const{ return Config.Gap; }
	FORCEINLINE void SetCrosshairColor(const FLinearColor& InColor) { Config.Color = InColor; }
	FORCEINLINE FLinearColor GetCrosshairColor() const { return Config.Color; }
	FORCEINLINE FCrosshairSettings GetDefaultConfig() const { return DefaultConfig; }
	
};
