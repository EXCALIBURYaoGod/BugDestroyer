// Copyright @FpsLuping all reserved


#include "Weapons/Projectiles/ProjectileBullet.h"

#include "DebugHelper.h"
#include "GameplayTagAssetInterface.h"
#include "Character/BugCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/Weapon.h"


// Sets default values
AProjectileBullet::AProjectileBullet()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
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
	if (HasAuthority())
	{	// server
		if (!bFakeProjectile)
		{
			ABugCharacter* BugCharacter = Cast<ABugCharacter>(OtherActor);
            if (BugCharacter)
            {
            	BugCharacter->GetHit(Hit.ImpactPoint);
            	UGameplayStatics::ApplyDamage(OtherActor, SelectedData->ImpactDamage, GetInstigatorController(), this, nullptr);
            }
		}
	}else if (bUseServerSideRewind)
	{	// Only Instigator生成fake projectile
		if (bFakeProjectile)
		{
			AWeapon* DamageCauser = Cast<AWeapon>(GetOwner());
			ABugCharacter* HitCharacter = Cast<ABugCharacter>(OtherActor);
			if (DamageCauser && HitCharacter)
			{
				ServerProjectileHitRequest(HitCharacter, 
					InitialSpawnLocation, 
					ProjectileMovementComponent->Velocity,
					GetWorld()->GetTimeSeconds(), 
					DamageCauser
					);
			}
		}
	}
	Super::OnHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, Hit);
}

