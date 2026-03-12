// Copyright @FpsLuping all reserved


#include "Pickups/AmmoPickup.h"

#include "Character/BugCharacter.h"


AAmmoPickup::AAmmoPickup()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAmmoPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAmmoPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedComponent)
	{
		OverlappedComponent->SetGenerateOverlapEvents(false);
	}
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (ABugCharacter* BugCharacter = Cast<ABugCharacter>(OtherActor))
	{
		BugCharacter->PickupAmmo(WeaponType, AmmoAmount);
	}
	
	SetLifeSpan(0.1f);
}


void AAmmoPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

