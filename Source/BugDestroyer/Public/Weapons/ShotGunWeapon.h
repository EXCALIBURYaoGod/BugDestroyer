// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "ShotGunWeapon.generated.h"

USTRUCT(BlueprintType)
struct FPelletHitInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FVector_NetQuantize HitPoint;
	UPROPERTY()
	FVector_NetQuantize TraceEnd;
	UPROPERTY()
	FVector_NetQuantizeNormal HitNormal;
	UPROPERTY()
	TWeakObjectPtr<AActor> HitActor; 
	
};

UCLASS()
class BUGDESTROYER_API AShotGunWeapon : public AHitScanWeapon
{
	GENERATED_BODY()

public:
	AShotGunWeapon();

protected:
	virtual void BeginPlay() override;
	// begin AWeapon Interface
	virtual void ServerExecuteFireLogic(const FVector& HitTarget, int32 InRandomSeed) override;
	virtual void SimulateFireFX(const FVector& HitTarget, int32 InRandomSeed) override;
	// end AWeapon Interface
	
	UFUNCTION(NetMulticast, Unreliable) 
	void Multicast_SpawnShotgunFX(const TArray<FPelletHitInfo>& OutHits);
	
private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	uint32 NumberOfPellets = 10;

	
public:
	virtual void Tick(float DeltaTime) override;
	
};
