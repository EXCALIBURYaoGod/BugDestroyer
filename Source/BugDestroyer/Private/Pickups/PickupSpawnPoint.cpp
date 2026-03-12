// Copyright @FpsLuping all reserved


#include "Pickups/PickupSpawnPoint.h"

#include "NiagaraComponent.h"
#include "Pickups/Pickup.h"


APickupSpawnPoint::APickupSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	PickupCircleComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("PickupCircleComponent"));
	
}

void APickupSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	SpawnPickupTimerFinished();
	
}

void APickupSpawnPoint::SpawnPickupByTag(FGameplayTag InGameplayTag)
{
	if (!HasAuthority() || !InGameplayTag.IsValid()) return;

	TSubclassOf<APickup>* FoundClassPtr = PickupClasses.Find(InGameplayTag);
	if (FoundClassPtr && *FoundClassPtr)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			FVector SpawnLocation = GetActorLocation(); 
			FRotator SpawnRotation = GetActorRotation();

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			SpawnedPickup = World->SpawnActor<APickup>(*FoundClassPtr, SpawnLocation, SpawnRotation, SpawnParams);
			if (SpawnedPickup)
			{
				SpawnedPickup->OnDestroyed.AddDynamic(this, &ThisClass::StartSpawnPickupTimer);
			}
		
		}
	}
}

void APickupSpawnPoint::RandomSpawnPickupByTag(FGameplayTag InGameplayTag)
{
	if (!HasAuthority() || !InGameplayTag.IsValid()) return;
	
	TArray<FGameplayTag> MatchingTags;
	
	for (auto& Elem : PickupClasses)
	{
		if (Elem.Key.MatchesTag(InGameplayTag))
		{
			MatchingTags.Add(Elem.Key);
		}
	}
	
	if (MatchingTags.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, MatchingTags.Num() - 1);
		FGameplayTag SelectedTag = MatchingTags[RandomIndex];
		
		SpawnPickupByTag(SelectedTag);
	}
}

void APickupSpawnPoint::SpawnPickupTimerFinished()
{

	SpawnPickupByTag(PickupTagToSpawn);
	RandomSpawnPickupByTag(PickupsTagToSpawn);
	
}

void APickupSpawnPoint::StartSpawnPickupTimer(AActor* DestoryedActor)
{
	const float SpawnTime = FMath::FRandRange(SpawnPickupTimeMin, SpawnPickupTimeMax);
	GetWorldTimerManager().SetTimer(
		SpawnPickupTimer,
		this,
		&ThisClass::SpawnPickupTimerFinished,
		SpawnTime
	);
	
}


void APickupSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

