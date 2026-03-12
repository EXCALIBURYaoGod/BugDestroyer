// Copyright @FpsLuping all reserved


#include "Pickups/HealthPickup.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/BugCharacter.h"

AHealthPickup::AHealthPickup()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
}

void AHealthPickup::BeginPlay()
{
	Super::BeginPlay();
	
}


void AHealthPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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
			if (BugCharacter->GetCurrentHealth() >= BugCharacter->GetMaxHealth())
			{
				if (OverlappedComponent)
				{
					OverlappedComponent->SetGenerateOverlapEvents(true);
				}
				return;
			}
			BugCharacter->Healing(HealAmount, HealingTime);
		}
	}
	
	SetLifeSpan(0.1f);
}

void AHealthPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

