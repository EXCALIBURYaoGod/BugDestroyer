// Copyright @FpsLuping all reserved


#include "Weapons/ProjectileRocket.h"

#include "DebugHelper.h"
#include "Character/BugCharacter.h"
#include "Kismet/GameplayStatics.h"


AProjectileRocket::AProjectileRocket()
{

	PrimaryActorTick.bCanEverTick = true;
}


void AProjectileRocket::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	const FImpactEffectData* SelectedData = &DefaultImpactData;
	FGameplayTagContainer TargetTags;
	if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(OtherActor))
	{
		TagInterface->GetOwnedGameplayTags(TargetTags);
		
		for (const auto& Pair : TaggedImpactEffects)
		{
			if (TargetTags.HasTag(Pair.Key))
			{
				SelectedData = &Pair.Value;
				break;
			}
		}
	}
	if (APawn* FiringPawn = GetInstigator())
	{
		if (AController* FiringController = FiringPawn->GetController())
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this,
				SelectedData->ImpactDamage,
				MinimumDamage,
				GetActorLocation(),
				DamageInnerRadius,
				DamageOuterRadius,
				DamageFalloff,
				UDamageType::StaticClass(),
				TArray<AActor*>(),
				this,
				FiringController
			);
		}
	}
	Debug::Print(FString::Printf(TEXT("OtherActor: %s"), *OtherActor->GetName()));
	Super::OnHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, Hit);
}


void AProjectileRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

