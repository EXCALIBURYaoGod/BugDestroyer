// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "ShieldPickup.generated.h"

UCLASS()
class BUGDESTROYER_API AShieldPickup : public APickup
{
	GENERATED_BODY()

public:
	AShieldPickup();
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
	UPROPERTY(EditAnywhere, Category= "Shield")
	float ShieldReplenishTime = 3.f;
	UPROPERTY(EditAnywhere, Category= "Health")
	float ShieldAmount = 100.f;
	
public:
	
	
};
