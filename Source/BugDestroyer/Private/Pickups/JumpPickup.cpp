// Copyright @FpsLuping all reserved


#include "Pickups/JumpPickup.h"

#include "Character/BugCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


AJumpPickup::AJumpPickup()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AJumpPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

void AJumpPickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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
				if (BugCharacter->GetCharacterMovement()->JumpZVelocity == JumpZVelocityBuff)
				{
					if (OverlappedComponent)
					{
						OverlappedComponent->SetGenerateOverlapEvents(true);
					}
					return;
				}
			}
			BugCharacter->BuffingJump(JumpZVelocityBuff, JumpBuffTime);
		}
	}
	
	SetLifeSpan(0.1f);
}

// Called every frame
void AJumpPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

