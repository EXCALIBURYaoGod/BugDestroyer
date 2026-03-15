// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

UCLASS()
class BUGDESTROYER_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()

public:
	AProjectileRocket();

protected:
	virtual void BeginPlay() override;
	void ApplyRadiaDamageFromImpactData(const FImpactEffectData* SelectedData);
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) override;
	void HandleFXDestruction();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FX")
	class UNiagaraComponent* TrailComponent;
	
private:
	UPROPERTY(EditAnywhere, Category="ProjectileRocket", meta=(AllowPrivateAccess=true))
	float MinimumDamage;
	UPROPERTY(EditAnywhere, Category="ProjectileRocket", meta=(AllowPrivateAccess=true))
	float DamageInnerRadius;
	UPROPERTY(EditAnywhere, Category="ProjectileRocket", meta=(AllowPrivateAccess=true))
	float DamageOuterRadius;
	UPROPERTY(EditAnywhere, Category="ProjectileRocket", meta=(AllowPrivateAccess=true))
	float DamageFalloff;
	UPROPERTY(VisibleAnywhere)
	UAudioComponent* ProjectileFlyLoopComponent;

	
	
public:
	virtual void Tick(float DeltaTime) override;
	
};
