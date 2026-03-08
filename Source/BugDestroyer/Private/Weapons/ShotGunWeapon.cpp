// Copyright @FpsLuping all reserved


#include "Weapons/ShotGunWeapon.h"

#include "GameplayTagAssetInterface.h"
#include "Character/BugCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


AShotGunWeapon::AShotGunWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AShotGunWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AShotGunWeapon::Fire(const FVector& HitTarget)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	AWeapon::Fire(HitTarget);
	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName("Muzzle");
	if (MuzzleSocket)
	{

		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		FVector MuzzleSocketLocation = SocketTransform.GetLocation();
		FVector LienTraceEnd;
		for (uint32 i = 0; i < NumberOfPellets; i++)
		{
			LienTraceEnd = TraceEndWithScatter(MuzzleSocketLocation, HitTarget);
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
						if (FireHit.bBlockingHit)
						{
							Beam->SetVectorParameter(FName("Target"), FireHit.ImpactPoint);
						}
						else
						{
							Beam->SetVectorParameter(FName("Target"), BeamEnd);
						}
					}
				}
				
			}
		}
		
		
	}
}



void AShotGunWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

