// Copyright @FpsLuping all reserved


#include "Pickups/SpeedPickup.h"

#include "Character/BugCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ASpeedPickup::ASpeedPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASpeedPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpeedPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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
			if (BugCharacter->GetCharacterMovement())
			{
				if (BugCharacter->GetCharacterMovement()->MaxWalkSpeed == BaseSpeedBuff)
				{
					if (OverlappedComponent)
					{
						OverlappedComponent->SetGenerateOverlapEvents(true);
					}
					return;
				}
			}
			BugCharacter->BuffingSpeed(BaseSpeedBuff, CrouchSpeedBuff, SpeedBuffTime);
		}
	}
	
	SetLifeSpan(0.1f);
}

// Called every frame
void ASpeedPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

