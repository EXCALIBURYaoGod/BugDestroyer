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

void AProjectileWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectileWeapon::ServerExecuteFireLogic(const FVector& HitTarget, int32 InRandomSeed)
{
	Super::ServerExecuteFireLogic(HitTarget, InRandomSeed);
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("Muzzle"));
    
	if (MuzzleSocket && ProjectileClass)
	{
		FTransform MuzzleTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		FVector ToTarget = HitTarget - MuzzleTransform.GetLocation();
		FRotator TargetRot = ToTarget.Rotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;
        
		// 纯逻辑：服务端生成物理抛射体。
		GetWorld()->SpawnActor<AProjectile>(
			ProjectileClass,
			MuzzleTransform.GetLocation(),
			TargetRot,
			SpawnParams
		);
	}
}

void AProjectileWeapon::SpawnFakeProjectile(const FVector& HitTarget)
{
	if (HasAuthority()) return;
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn && !OwnerPawn->IsLocallyControlled())
	{
		return;
	}
	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("Muzzle"));
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	if (MuzzleSocket)
	{
		FTransform MuzzleTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		FVector ToTarget = HitTarget - MuzzleTransform.GetLocation();
		FRotator TargetRot = ToTarget.Rotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;
		if (FakeProjectileClass)
		{
			 GetWorld()->SpawnActor<AProjectile>(FakeProjectileClass, MuzzleTransform.GetLocation(), TargetRot);
		}
	}
}

void AProjectileWeapon::SimulateFireFX(const FVector& HitTarget, int32 InRandomSeed)
{
	Super::SimulateFireFX(HitTarget, InRandomSeed);
	SpawnFakeProjectile(HitTarget);
	
}



