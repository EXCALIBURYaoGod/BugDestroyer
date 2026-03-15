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

void AShotGunWeapon::ServerExecuteFireLogic(const FVector& HitTarget, int32 InRandomSeed)
{
	AWeapon::ServerExecuteFireLogic(HitTarget, InRandomSeed);
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	
	TArray<FPelletHitInfo> OutHits;

	for (uint32 i = 0; i < NumberOfPellets; i++)
	{
		FHitResult TraceHit;
		FVector TraceEnd = PerformHitScanTrace(OwnerPawn, HitTarget, TraceHit, InRandomSeed + i);
		FPelletHitInfo Info;
		if (TraceHit.bBlockingHit)
		{
			ApplyDamageByTag(OwnerPawn, TraceHit);
			
			Info.HitPoint = TraceHit.ImpactPoint;
			Info.HitNormal = TraceHit.ImpactNormal;
			Info.HitActor = TraceHit.GetActor();
		}
		Info.TraceEnd = TraceEnd;
		OutHits.Add(Info);
	}
	
	if (OutHits.Num() > 0)
	{
		Multicast_SpawnShotgunFX(OutHits);
	}
}

void AShotGunWeapon::SimulateFireFX(const FVector& HitTarget, int32 InRandomSeed)
{
	Super::SimulateFireFX(HitTarget, InRandomSeed);
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn && OwnerPawn->IsLocallyControlled())
	{
		if (UWorld* World = GetWorld())
		{
			const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName("Muzzle");
			if (!MuzzleSocket) return;
			FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
			FVector MuzzleSocketLocation = SocketTransform.GetLocation();
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
			TArray<FPelletHitInfo> OutHits;
			for (uint32 i = 0; i < NumberOfPellets; i++)
			{
				FHitResult TraceHit;
				FVector TraceEnd = PerformHitScanTrace(OwnerPawn, HitTarget, TraceHit, InRandomSeed + i);
				FPelletHitInfo Info;
				if (TraceHit.bBlockingHit)
				{
					Info.HitPoint = TraceHit.ImpactPoint;
					Info.HitNormal = TraceHit.ImpactNormal;
					Info.HitActor = TraceHit.GetActor();
				}
				Info.TraceEnd = TraceEnd;
				OutHits.Add(Info);
			}
	
			if (OutHits.Num() > 0)
			{
				for (const FPelletHitInfo& Pellet : OutHits)
				{
					SpawnImpactEffect(Pellet.HitPoint, Pellet.HitNormal, Pellet.HitActor.Get());
					SpawnBeamFX(Pellet.TraceEnd);
				}
			}
		}
	}
}

void AShotGunWeapon::Multicast_SpawnShotgunFX_Implementation(const TArray<FPelletHitInfo>& OutHits)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn && OwnerPawn->IsLocallyControlled()) return;
	for (const FPelletHitInfo& Pellet : OutHits)
	{
		SpawnImpactEffect(Pellet.HitPoint, Pellet.HitNormal, Pellet.HitActor.Get());
		SpawnBeamFX(Pellet.TraceEnd);
	}
}




void AShotGunWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

