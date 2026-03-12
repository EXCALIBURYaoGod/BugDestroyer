// Copyright @FpsLuping all reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "PickupSpawnPoint.generated.h"

UCLASS()
class BUGDESTROYER_API APickupSpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	APickupSpawnPoint();
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, Category = "PickupClasses", meta = (Categories = "Gameplay.PickupAmmo,Gameplay.PickupBuff"))
	TMap<FGameplayTag , TSubclassOf<class APickup>> PickupClasses;
	
	void SpawnPickupByTag(FGameplayTag InGameplayTag);
	void RandomSpawnPickupByTag(FGameplayTag InGameplayTag);
	void SpawnPickupTimerFinished();
	UFUNCTION()
	void StartSpawnPickupTimer(AActor* DestoryedActor);
	
private:
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* PickupCircleComponent;
	UPROPERTY(EditAnywhere, Category = "PickupClasses", meta = (Categories = "Gameplay.PickupAmmo,Gameplay.PickupBuff"))
	FGameplayTag PickupTagToSpawn;
	UPROPERTY(EditAnywhere, Category = "PickupClasses", meta = (Categories = "Gameplay.PickupAmmo,Gameplay.PickupBuff"))
	FGameplayTag PickupsTagToSpawn; // 宽范围，用于随机生成一个group的pickup
	UPROPERTY(EditAnywhere, Category = "SpawnTime")
	float SpawnPickupTimeMin;
	UPROPERTY(EditAnywhere, Category = "SpawnTime")
	float SpawnPickupTimeMax;
	UPROPERTY()
	APickup* SpawnedPickup;
	FTimerHandle SpawnPickupTimer;
	
	
public:
	
	
	
};
