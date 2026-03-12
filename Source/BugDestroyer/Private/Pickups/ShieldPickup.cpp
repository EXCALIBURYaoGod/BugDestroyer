// Copyright @FpsLuping all reserved


#include "Pickups/ShieldPickup.h"

#include "Character/BugCharacter.h"


// Sets default values
AShieldPickup::AShieldPickup()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AShieldPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShieldPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedComponent)
	{
		OverlappedComponent->SetGenerateOverlapEvents(false);
	}
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (ABugCharacter* BugCharacter = Cast<ABugCharacter>(OtherActor))
	{
		if (BugCharacter->HasAuthority())
		{
			if (BugCharacter->GetCurrentShield() >= BugCharacter->GetMaxShield())
			{
				if (OverlappedComponent)
				{
					OverlappedComponent->SetGenerateOverlapEvents(true);
				}
				return;
			}
			BugCharacter->ShieldReplenish(ShieldAmount, ShieldReplenishTime);
		}
	}
	
	SetLifeSpan(0.1f);
}

// Called every frame
void AShieldPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

