// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "ShotGunWeapon.generated.h"

UCLASS()
class BUGDESTROYER_API AShotGunWeapon : public AHitScanWeapon
{
	GENERATED_BODY()

public:
	AShotGunWeapon();

protected:
	virtual void BeginPlay() override;
	// begin AWeapon Interface
	virtual void Fire(const FVector& HitTarget) override;
	// end AWeapon Interface
	
private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	uint32 NumberOfPellets = 10;
	
public:
	virtual void Tick(float DeltaTime) override;
	
};
