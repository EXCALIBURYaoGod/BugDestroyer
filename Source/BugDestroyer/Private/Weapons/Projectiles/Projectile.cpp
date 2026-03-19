// //Copyrights @FpsLuping all reserved


#include "Weapons/Projectiles/Projectile.h"

#include "GameplayTagAssetInterface.h"
#include "BugDestroyer/BugDestroyer.h"
#include "Character/BugCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/LagCompensationComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/Weapon.h"


AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECC_Projectile);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SKM, ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_HitBox, ECR_Block); // 改为仅对HitBox生效
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->InitialSpeed = ProjectileInitSpeed;
	ProjectileMovementComponent->MaxSpeed = ProjectileMaxSpeed;
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	StaticMesh->SetupAttachment(CollisionBox);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}


void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetNetMode() != NM_DedicatedServer)
	{
		if (Tracer)
		{
			TracerComponent = UGameplayStatics::SpawnEmitterAttached(
					Tracer,
					CollisionBox,
					FName(),
					GetActorLocation(),
					GetActorRotation(),
					EAttachLocation::KeepWorldPosition
				);
		}
	}

	if (AActor* MyOwner = GetOwner())
	{
		CollisionBox->IgnoreActorWhenMoving(MyOwner, true);
	}
	
	CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
	
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	FVector NormalImpulse, const FHitResult& Hit)
{
	const FImpactEffectData* SelectedData = GetHitImpactDataByHitActorOwnTag(OtherActor);
	
	if (GetNetMode() != NM_DedicatedServer) 
	{
		bool bIsLocalShooter = false;
		if (APawn* InstigatorPawn = GetInstigator())
		{
			bIsLocalShooter = InstigatorPawn->IsLocallyControlled();
		}
		
		if (bFakeProjectile || (!bFakeProjectile && !bIsLocalShooter) || HasAuthority())
		{
			if (SelectedData->Particles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedData->Particles, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}
    
			if (SelectedData->Sound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, SelectedData->Sound, Hit.ImpactPoint);
			}
		}
	}
	
	if (bFakeProjectile)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StaticMesh->SetVisibility(false);
		SetLifeSpan(0.1f);
	}
	else if (HasAuthority()) 
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StaticMesh->SetVisibility(false);
		SetLifeSpan(0.1f); 
	}
}

// 隐藏开枪者的服务器同步子弹
bool AProjectile::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	
	if (GetInstigator() && GetInstigator()->GetController() == RealViewer)
	{
		return false;
	}

	return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
}

const FImpactEffectData* AProjectile::GetHitImpactDataByHitActorOwnTag(AActor* OtherActor)
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
	return SelectedData;
	
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::ServerProjectileHitRequest_Implementation(ABugCharacter* HitCharacter,
	const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& InitialVelocity, float HitTime,
	class AWeapon* DamageCauser)
{
	if (ABugCharacter* ProjectileInstigator = Cast<ABugCharacter>(GetInstigator()))
	{
		if (ULagCompensationComponent* LagCompensationComponent = ProjectileInstigator->GetLagCompensationComponent())
		{
			FServerSideRewindResult ServerSideRewindResult = LagCompensationComponent->ServerSideRewind_Projectile(HitCharacter, TraceStart, InitialVelocity, HitTime);
          
			if (HitCharacter && ServerSideRewindResult.HitResult.bBlockingHit)
			{
				float Damage = DamageCauser->GetHitImpactDataByHitActorOwnTag(HitCharacter)->ImpactDamage;
				FVector ShotDirection = InitialVelocity.GetSafeNormal();
				UGameplayStatics::ApplyPointDamage(
				   HitCharacter,
				   Damage,
				   ShotDirection,
				   ServerSideRewindResult.HitResult, 
				   ProjectileInstigator->GetController(),
				   DamageCauser,
				   UDamageType::StaticClass()
				);
			}
		}
	}
}


#if WITH_EDITOR
void AProjectile::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	FName PropertyName = PropertyChangedEvent.Property != nullptr? PropertyChangedEvent.Property->GetFName(): NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AProjectile, ProjectileInitSpeed) || PropertyName == GET_MEMBER_NAME_CHECKED(AProjectile, ProjectileMaxSpeed))
	{
		if (ProjectileMovementComponent)
		{
			ProjectileMovementComponent->InitialSpeed = ProjectileInitSpeed;
			ProjectileMovementComponent->MaxSpeed = ProjectileMaxSpeed;
		}
	}
}
#endif




