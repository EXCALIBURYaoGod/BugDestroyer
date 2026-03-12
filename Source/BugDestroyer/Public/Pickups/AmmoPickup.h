// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "Weapons/Weapon.h"
#include "AmmoPickup.generated.h"

UCLASS()
class BUGDESTROYER_API AAmmoPickup : public APickup
{
	GENERATED_BODY()

public:
	AAmmoPickup();

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
	UPROPERTY(EditAnywhere, Category="Ammos")
	int32 AmmoAmount;
	UPROPERTY(EditAnywhere, Category="Weapon")
	EWeaponType WeaponType;
	
public:
	virtual void Tick(float DeltaTime) override;
	
};
