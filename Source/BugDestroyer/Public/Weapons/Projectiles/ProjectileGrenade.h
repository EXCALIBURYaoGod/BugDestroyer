// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileGrenade.generated.h"

UCLASS()
class BUGDESTROYER_API AProjectileGrenade : public AProjectile
{
	GENERATED_BODY()

public:
	AProjectileGrenade();

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	UFUNCTION()
	void OnHandleBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UNiagaraComponent* TrailComponent;
	
	UPROPERTY(EditAnywhere, Category= "Grenade Property")
	float GrenadeTriggerTime = 5.f;
	UPROPERTY(EditAnywhere, Category = "Grenade|Damage")
	float BaseDamage = 120.f;
	UPROPERTY(EditAnywhere, Category = "Grenade|Damage")
	float MinimumDamage = 30.f;
	UPROPERTY(EditAnywhere, Category = "Grenade|Damage")
	float DamageInnerRadius = 250.f; 
	UPROPERTY(EditAnywhere, Category = "Grenade|Damage")
	float DamageOuterRadius = 700.f; 
	UPROPERTY(EditAnywhere, Category = "Grenade|Damage")
	float DamageFalloff = 1.f; 
	
	UPROPERTY(EditAnywhere, Category = "Grenade|FX")
	UParticleSystem* ExplosionParticles;
	UPROPERTY(EditAnywhere, Category = "Grenade|FX")
	USoundBase* BounceSound;
	
private:

	
public:
	virtual void Tick(float DeltaTime) override;
	
	
};
