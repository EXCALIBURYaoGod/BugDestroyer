// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class BUGDESTROYER_API APickup : public AActor
{
	GENERATED_BODY()

public:
	APickup();
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	
private:
	UPROPERTY(EditAnywhere, Category="Components")
	class USphereComponent* OverlapSphere;
	UPROPERTY(EditAnywhere, Category="Components")
	UStaticMeshComponent* PickupMesh;
	UPROPERTY(EditAnywhere, Category= "Components")
	class UNiagaraComponent* VisualComponent; // use fx as mesh
	UPROPERTY(EditAnywhere, Category="FX")
	class USoundBase* PickupSound;
	UPROPERTY(EditAnywhere, Category="FX")
	float BaseRotateRate = 45.f;
	UPROPERTY(EditAnywhere, Category= "FX")
	class UNiagaraSystem* PickupEffect;
	
public:

	
};
