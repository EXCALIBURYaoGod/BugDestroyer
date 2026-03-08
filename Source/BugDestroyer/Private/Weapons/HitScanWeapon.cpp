// Copyright @FpsLuping all reserved


#include "Weapons/HitScanWeapon.h"

#include "Character/BugCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"


AHitScanWeapon::AHitScanWeapon()
{

}


void AHitScanWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	
	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName("Muzzle");
	if (MuzzleSocket)
	{
		
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		FVector MuzzleSocketLocation = SocketTransform.GetLocation();
		FVector LienTraceEnd = MuzzleSocketLocation + (HitTarget - MuzzleSocketLocation) * 1.25f;
		FHitResult FireHit;
		UWorld* World = GetWorld();
		if (World)
		{
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this); 
			QueryParams.AddIgnoredActor(OwnerPawn);
			World->LineTraceSingleByChannel(
				FireHit,
				MuzzleSocketLocation,
				LienTraceEnd,
				ECC_Visibility,
				QueryParams
				);
			FVector BeamEnd = LienTraceEnd;
			if (FireHit.bBlockingHit)
			{
				const FImpactEffectData* SelectedData = &DefaultImpactData;
				FGameplayTagContainer TargetTags;
				if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(FireHit.GetActor()))
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
				ABugCharacter* BugCharacter = Cast<ABugCharacter>(FireHit.GetActor());
				if (BugCharacter)
				{
					AController* InstigatorController = OwnerPawn->GetController();
					if (HasAuthority() && InstigatorController)
					{
						BugCharacter->GetHit(FireHit.ImpactPoint);
						UGameplayStatics::ApplyDamage(
                        	BugCharacter,
                        	SelectedData->ImpactDamage,
                        	InstigatorController,
                        	this,
                        	UDamageType::StaticClass()
                        	);
					}
					
				}
				if (SelectedData->Particles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(
						World,
						SelectedData->Particles,
						FireHit.ImpactPoint,
						FireHit.ImpactNormal.Rotation()
					);
				}
				if (SelectedData->Sound)
				{
					UGameplayStatics::PlaySoundAtLocation(
						this,
						SelectedData->Sound,
						FireHit.ImpactPoint
					);
				}
				
			}
			if (BeamParticle)
			{
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
					World,
					BeamParticle,
					SocketTransform
				);
				if (Beam)
				{
					Beam->SetVectorParameter(FName("Target"), BeamEnd);
				}
			}

			if (MuzzleFlash)
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					World,
					MuzzleFlash,
					SocketTransform
				);
			}
			if (FireSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					this,
					FireSound,
					MuzzleSocketLocation
				);
			}
			
		}
		
		
	}
	
}

FVector AHitScanWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget)
{
	FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	FVector RandVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
	FVector EndLoc = SphereCenter + RandVector;
	FVector ToEndLoc = EndLoc - TraceStart;
	return FVector(TraceStart + ToEndLoc * 80000.f / ToEndLoc.Size());
}




