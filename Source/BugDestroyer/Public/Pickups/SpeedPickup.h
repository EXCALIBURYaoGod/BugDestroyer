// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "SpeedPickup.generated.h"

UCLASS()
class BUGDESTROYER_API ASpeedPickup : public APickup
{
	GENERATED_BODY()

public:
	ASpeedPickup();
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;
	
	//begin APickup Interface
	virtual void OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
	) override;
	//end APickup Interface
	
private:
	UPROPERTY(EditAnywhere, Category="Speed")
	float BaseSpeedBuff = 1600.f;
	UPROPERTY(EditAnywhere, Category="Speed")
	float CrouchSpeedBuff = 850.f;
	UPROPERTY(EditAnywhere, Category="Speed")
	float SpeedBuffTime = 15.f;
	
public:
	
};
