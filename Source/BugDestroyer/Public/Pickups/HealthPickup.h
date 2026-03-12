// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "HealthPickup.generated.h"

UCLASS()
class BUGDESTROYER_API AHealthPickup : public APickup
{
	GENERATED_BODY()

public:
	AHealthPickup();
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
	UPROPERTY(EditAnywhere, Category= "Health")
	float HealingTime = 3.f;
	UPROPERTY(EditAnywhere, Category= "Health")
	float HealAmount = 100.f;
	
public:

	
};
