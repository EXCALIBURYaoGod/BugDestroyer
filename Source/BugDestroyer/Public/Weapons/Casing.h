// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

/**
 * 弹壳类，作为弹壳弹出的实体
 */
UCLASS()
class BUGDESTROYER_API ACasing : public AActor
{
	GENERATED_BODY()

public:
	ACasing();
	
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;
	UPROPERTY(EditAnywhere)
	float ShellEjectionImpulse;
	UPROPERTY(EditAnywhere)
	USoundBase* ShellSound;
	
public:
	
};
