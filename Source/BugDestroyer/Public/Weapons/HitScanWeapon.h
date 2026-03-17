// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Weapon.h"
#include "BugTypes/BugStructTypes.h"
#include "HitScanWeapon.generated.h"


/**
 * 射线检测武器，无需子弹，没有子弹下坠，模拟弹道散布
 */
UCLASS()
class BUGDESTROYER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	AHitScanWeapon();

protected:
	virtual void BeginPlay() override;

	// begin AWeapon Interface
	virtual void ServerExecuteFireLogic(const FVector& HitTarget, int32 InRandomSeed) override;
	virtual void SimulateFireFX(const FVector& HitTarget, int32 InRandomSeed) override;
	// end AWeapon Interface
	
	UFUNCTION(Server, Reliable)
	void ServerHitRequest(ABugCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime, class AWeapon* DamageCauser);
	void ApplyDamageByTag(APawn* OwnerPawn, FHitResult TraceHit);
	FVector GetMuzzleSocketLocation();
	FVector PerformHitScanTrace(const APawn* OwnerPawn, const FVector& HitTarget, FHitResult& TraceHit, int32 InRandomSeed);
	UFUNCTION(NetMulticast, UnReliable)
	void Multicast_SpawnImpactFX(const FVector& Point, const FVector& Normal, AActor* HitActor);
	UFUNCTION(NetMulticast, UnReliable)
	void Multicast_SpawnBeamFX(const FVector& TraceEnd);
	void SpawnBeamFX(const FVector& TraceEnd);
	void SpawnImpactEffect(const FVector& Point, const FVector& Normal, AActor* HitActor);
	
	UPROPERTY(EditAnywhere, Category = "ScanEffects")
	UParticleSystem* BeamParticle;
	UPROPERTY(EditAnywhere, Category = "ScanEffects")
	UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere, Category = "ScanEffects")
	USoundBase* FireSound;

	
private:

	
public:
	FORCEINLINE float GetImpactDamageFromTag(FGameplayTag Tag) const { return TaggedImpactEffects.Contains(Tag) ? TaggedImpactEffects[Tag].ImpactDamage : 0.f; }
	FORCEINLINE void SetFireSphereRadius(const float InRadius) { FireSphereRadius = InRadius; }
	FORCEINLINE float GetFireSphereRadius() const{ return FireSphereRadius; }
	FORCEINLINE float GetDefaultFireSphereRadius() const{ return DefaultFireSphereRadius; }
	
};
