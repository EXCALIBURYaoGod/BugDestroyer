// //Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BugTypes/BugStructTypes.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UProjectileMovementComponent;
class UBoxComponent;

UCLASS()
class BUGDESTROYER_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();
	// begin AActor Interface
	virtual void Tick(float DeltaTime) override;
	// end AActor Interface
	
	
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	
	UPROPERTY(EditAnywhere, Category = "ImpactEffects")
	FImpactEffectData DefaultImpactData;
	UPROPERTY(EditAnywhere, Category = "ImpactEffects")
	TMap<FGameplayTag, FImpactEffectData> TaggedImpactEffects;
	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBox;
	
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(EditAnywhere)
	UParticleSystem* Tracer;
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* TracerComponent;


	
public:
	FORCEINLINE float GetImpactDamageFromTag(FGameplayTag Tag) const { return TaggedImpactEffects.Contains(Tag) ? TaggedImpactEffects[Tag].ImpactDamage : 0.f; }
	
};
