// //Copyrights @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

class AProjectile;

/**
 * 子弹武器类，作为可以发射子弹的武器
 */
UCLASS()
class BUGDESTROYER_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	AProjectileWeapon();
	virtual void Tick(float DeltaTime) override;
	
	// begin AWeapon Interface
	virtual void ServerExecuteFireLogic(const FVector& HitTarget, int32 InRandomSeed) override;
	void SpawnFakeProjectile(const FVector& HitTarget);
	virtual void SimulateFireFX(const FVector& HitTarget, int32 InRandomSeed) override;
	// end AWeapon Interface
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category= "Projectiles")
	TSubclassOf<AProjectile> ProjectileClass;
	UPROPERTY(EditAnywhere, Category= "Projectiles")
	TSubclassOf<AProjectile> FakeProjectileClass;
	
public:
	
};
