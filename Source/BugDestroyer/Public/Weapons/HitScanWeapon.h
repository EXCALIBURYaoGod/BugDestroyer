// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Weapon.h"
#include "BugTypes/BugStructTypes.h"
#include "HitScanWeapon.generated.h"

/**
 * 射线检测武器，无需子弹，没有子弹下坠
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
	virtual void Fire(const FVector& HitTarget) override;
	// end AWeapon Interface
	
	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);
	UPROPERTY(EditAnywhere, Category = "ImpactEffects")
	FImpactEffectData DefaultImpactData;
	UPROPERTY(EditAnywhere, Category = "ImpactEffects")
	TMap<FGameplayTag, FImpactEffectData> TaggedImpactEffects;
	UPROPERTY(EditAnywhere, Category = "ScanEffects")
	UParticleSystem* BeamParticle;
	UPROPERTY(EditAnywhere, Category = "ScanEffects")
	UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere, Category = "ScanEffects")
	USoundBase* FireSound;
	
	
private:
	
	// trace end with scatter
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float DistanceToSphere = 800.f;
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	float SphereRadius = 75.f;
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	bool bUseScatter = false;
	
public:
	FORCEINLINE float GetImpactDamageFromTag(FGameplayTag Tag) const { return TaggedImpactEffects.Contains(Tag) ? TaggedImpactEffects[Tag].ImpactDamage : 0.f; }
	
	
};
