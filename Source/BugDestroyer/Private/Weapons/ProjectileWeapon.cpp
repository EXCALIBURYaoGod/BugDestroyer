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
		FVector TraceEnd = bUseScatter ? TraceEndWithScatter(MuzzleTransform.GetLocation(), HitTarget, InRandomSeed) : HitTarget;
		FVector ToTarget = TraceEnd - MuzzleTransform.GetLocation();
		FRotator TargetRot = ToTarget.Rotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
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

void AProjectileWeapon::SpawnFakeProjectile(const FVector& HitTarget, int32 InRandomSeed)
{
	APawn* OwningPawn = Cast<APawn>(GetOwner());
	// 拦截条件：如果是服务器 || 拥有者为空 || 拥有者不是本地控制的（即模拟代理）
	if (HasAuthority() || OwningPawn == nullptr || !OwningPawn->IsLocallyControlled()) 
	{
		return; 
	}
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
		FVector TraceEnd = bUseScatter ? TraceEndWithScatter(MuzzleTransform.GetLocation(), HitTarget, InRandomSeed) : HitTarget;
		FVector ToTarget = TraceEnd - MuzzleTransform.GetLocation();
		FRotator TargetRot = ToTarget.Rotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = InstigatorPawn;
		if (FakeProjectileClass)
		{
			AProjectile* FakeProjectile = GetWorld()->SpawnActor<AProjectile>(FakeProjectileClass, MuzzleTransform.GetLocation(), TargetRot, SpawnParams);
			FakeProjectile->SetInitialSpawnLocation(MuzzleTransform.GetLocation()); //初始化位置以用于SSR
			FakeProjectile->bUseServerSideRewind = bUseServerSideRewind;
		}
	}
}

void AProjectileWeapon::SimulateFireFX(const FVector& HitTarget, int32 InRandomSeed)
{
	Super::SimulateFireFX(HitTarget, InRandomSeed);
	SpawnFakeProjectile(HitTarget, InRandomSeed);
	
}



