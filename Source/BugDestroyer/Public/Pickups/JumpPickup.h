// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "JumpPickup.generated.h"

UCLASS()
class BUGDESTROYER_API AJumpPickup : public APickup
{
	GENERATED_BODY()

public:
	AJumpPickup();
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
	UPROPERTY(EditAnywhere, Category= "Jump")
	float JumpZVelocityBuff = 4000.f;
	UPROPERTY(EditAnywhere, Category= "Jump")
	float JumpBuffTime = 15.f;
	
public:
	
	
};
