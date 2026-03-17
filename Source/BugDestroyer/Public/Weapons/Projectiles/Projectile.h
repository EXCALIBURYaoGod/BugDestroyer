// //Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BugTypes/BugStructTypes.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class ABugCharacter;
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
	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;
	
	const FImpactEffectData* GetHitImpactDataByHitActorOwnTag(AActor* OtherActor);
	UPROPERTY(EditAnywhere, Category = "ImpactEffects")
	FImpactEffectData DefaultImpactData;
	UPROPERTY(EditAnywhere, Category = "ImpactEffects")
	TMap<FGameplayTag, FImpactEffectData> TaggedImpactEffects;
	UPROPERTY(EditAnywhere)
	UBoxComponent* CollisionBox;
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(EditAnywhere, Category = "ImpactEffects")
	bool bFakeProjectile;
	UPROPERTY(EditAnywhere, Category = "Speed")
	float ProjectileInitSpeed;
	UPROPERTY(EditAnywhere, Category = "Speed")
	float ProjectileMaxSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SSR")
	FVector InitialSpawnLocation;
	UFUNCTION(Server, Reliable)
	void ServerProjectileHitRequest(ABugCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& InitialVelocity, float HitTime, class AWeapon* DamageCauser);
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;
	UPROPERTY(EditAnywhere)
	UParticleSystem* Tracer;
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* TracerComponent;


	
public:
	FORCEINLINE float GetImpactDamageFromTag(FGameplayTag Tag) const { return TaggedImpactEffects.Contains(Tag) ? TaggedImpactEffects[Tag].ImpactDamage : 0.f; }
	FORCEINLINE void SetInitialSpawnLocation(const FVector& SpawnLocation) { InitialSpawnLocation = SpawnLocation; }
	UPROPERTY(EditAnywhere, Category = "ServerSideRewind")
	bool bUseServerSideRewind;
	
};
