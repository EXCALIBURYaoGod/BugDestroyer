// //Copyrights @FpsLuping all reserved


#include "Weapons/ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapons/Projectiles/Projectile.h"



AProjectileWeapon::AProjectileWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	
}


void AProjectileWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectileWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);
	
	// SpawnProjectile OnlyServerDo
	if (HasAuthority())
	{
		APawn* InstigatorPawn = Cast<APawn>(GetOwner());
		const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("Muzzle"));
		if (MuzzleSocket)
		{
			FTransform MuzzleTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
			FVector ToTarget = HitTarget - MuzzleTransform.GetLocation();
			FRotator TargetRot = ToTarget.Rotation();
			if (ProjectileClass)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = InstigatorPawn;
				UWorld* World = GetWorld();
				if (World)
				{
					World->SpawnActor<AProjectile>(
							ProjectileClass,
							MuzzleTransform.GetLocation(),
							TargetRot,
							SpawnParams
						);
				}
			}
		}
	}
	// SpawnProjectile OnlyServerDo
	
}

