// Copyright @FpsLuping all reserved


#include "Weapons/HitScanWeapon.h"

#include "Character/BugCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"


AHitScanWeapon::AHitScanWeapon()
{
	FireSphereRadius = DefaultFireSphereRadius;
}


void AHitScanWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AHitScanWeapon::Multicast_SpawnImpactFX_Implementation(const FVector& Point, const FVector& Normal,
	AActor* HitActor)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn && OwnerPawn->IsLocallyControlled()) return;
	SpawnImpactEffect(Point, Normal, HitActor);
}

void AHitScanWeapon::ServerExecuteFireLogic(const FVector& HitTarget, int32 InRandomSeed)
{
	Super::ServerExecuteFireLogic(HitTarget, InRandomSeed);
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;
	
	FHitResult TraceHit;
	FVector TraceEnd = PerformHitScanTrace(OwnerPawn, HitTarget, TraceHit, InRandomSeed);
	
	if (TraceHit.bBlockingHit)
	{
		// === 纯逻辑：造成伤害 ===
		ApplyDamageByTag(OwnerPawn, TraceHit);
		Multicast_SpawnImpactFX(TraceHit.ImpactPoint, TraceHit.ImpactNormal, TraceHit.GetActor());
	}
	
	Multicast_SpawnBeamFX(TraceEnd);
}

void AHitScanWeapon::SimulateFireFX(const FVector& HitTarget, int32 InRandomSeed)
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
			FHitResult PredictionHit;
			FVector TraceEnd = PerformHitScanTrace(OwnerPawn, HitTarget, PredictionHit, InRandomSeed);
			SpawnBeamFX(TraceEnd);
			if (PredictionHit.bBlockingHit)
			{
				SpawnImpactEffect(PredictionHit.ImpactPoint, PredictionHit.ImpactNormal, PredictionHit.GetActor());
			}
		}
	}
	
	
}

FVector AHitScanWeapon::PerformHitScanTrace(const APawn* OwnerPawn, const FVector& HitTarget, FHitResult& TraceHit, int32 InRandomSeed)
{
	FVector LineTraceEnd = FVector::ZeroVector;
	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName("Muzzle");
	if (MuzzleSocket)
	{
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		FVector MuzzleSocketLocation = SocketTransform.GetLocation();
		LineTraceEnd = bUseScatter? 
		TraceEndWithScatter(MuzzleSocketLocation, HitTarget, InRandomSeed) : MuzzleSocketLocation + (HitTarget - MuzzleSocketLocation) * 1.25f;
		UWorld* World = GetWorld();
		if (World)
		{
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this); 
			QueryParams.AddIgnoredActor(OwnerPawn);
			World->LineTraceSingleByChannel(
				TraceHit,
				MuzzleSocketLocation,
				LineTraceEnd,
				ECC_Visibility,
				QueryParams
			);
		}
	}
	if (TraceHit.bBlockingHit)
	{
		LineTraceEnd = TraceHit.ImpactPoint;
	}
	return LineTraceEnd;
}


void AHitScanWeapon::ApplyDamageByTag(APawn* OwnerPawn, FHitResult TraceHit)
{
	const FImpactEffectData* SelectedData = GetHitImpactDataByHitActorOwnTag(TraceHit.GetActor());
	ABugCharacter* BugCharacter = Cast<ABugCharacter>(TraceHit.GetActor());
	if (BugCharacter)
	{
		AController* InstigatorController = OwnerPawn->GetController();
		if (HasAuthority() && InstigatorController)
		{
			BugCharacter->GetHit(TraceHit.ImpactPoint);
			UGameplayStatics::ApplyDamage(
				BugCharacter,
				SelectedData->ImpactDamage,
				InstigatorController,
				this,
				UDamageType::StaticClass()
			);
		}
	}
}

void AHitScanWeapon::SpawnBeamFX(const FVector& TraceEnd)
{
	if (UWorld* World = GetWorld())
	{
		if (BeamParticle)
		{
			const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName("Muzzle");
			if (MuzzleSocket)
			{
				FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				World,
				BeamParticle,
				SocketTransform
			);
				if (Beam)
				{
					Beam->SetVectorParameter(FName("Target"), TraceEnd);
				}
			}
		}
	}
	
}

void AHitScanWeapon::SpawnImpactEffect(const FVector& Point, const FVector& Normal, AActor* HitActor)
{
	const FImpactEffectData* SelectedData = GetHitImpactDataByHitActorOwnTag(HitActor);
	if (UWorld* World = GetWorld())
	{
		if (SelectedData->Particles)
        	{
        		UGameplayStatics::SpawnEmitterAtLocation(
        			World,
        			SelectedData->Particles,
        			Point,
        			Normal.Rotation()
        		);
        	}
        	if (SelectedData->Sound)
        	{
        		UGameplayStatics::PlaySoundAtLocation(
        			this,
        			SelectedData->Sound,
        			Point
        		);
        	}
	}
	
}

void AHitScanWeapon::Multicast_SpawnBeamFX_Implementation(const FVector& TraceEnd)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn && OwnerPawn->IsLocallyControlled()) return;
	SpawnBeamFX(TraceEnd);
}

FVector AHitScanWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget, int32 InRandomSeed)
{
	FRandomStream WeaponStream(InRandomSeed);
	FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	FVector RandVector = WeaponStream.GetUnitVector() * WeaponStream.FRandRange(0.f, FireSphereRadius);
	FVector EndLoc = SphereCenter + RandVector;
	FVector ToEndLoc = EndLoc - TraceStart;
	return TraceStart + ToEndLoc.GetSafeNormal() * 80000.f;
}





